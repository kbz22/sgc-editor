#include "win32_models/symbol_tileset_control.hpp"
#include "win32_helpers/resize_helpers.hpp"
#include "program/program.hpp"
#include <algorithm>
#include <sgc/coordinates/screenworld.hpp>

bool win32_models::SymbolTilesetControl::m_classRegistered = false;
const wchar_t* g_className = L"SymbolTilesetControl";

win32_models::SymbolTilesetControl::SymbolTilesetControl(program::ProgramContext &programContext) :
    m_pointerManager{*programContext.GetManager<program::PointerManager>()}
{
    if(!m_classRegistered) 
    {
        WNDCLASSEX wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = DefWindowProc;
        wc.hInstance = programContext.GetHInstance();
        wc.lpszClassName = g_className;
        RegisterClassEx(&wc);

        m_classRegistered = true;
    }
}

void win32_models::SymbolTilesetControl::Init(HWND hwnd, program::ProgramContext &programContext)
{
    m_hwnd = hwnd;
    m_tilesetView = std::make_unique<sgc_view::SymbolTilesetView>(m_hwnd);

    SetWindowSubclass(
        m_hwnd,
        SymbolTilesetViewStaticProc,
        0,
        reinterpret_cast<DWORD_PTR>(this)
    );

    auto mapDocument = programContext.GetManager<file::FileManager>()->GetActiveDocument();
    RECT rect;
    GetWindowRect(hwnd, &rect);

    m_tilesetView->SetScreenSize(rect.right - rect.left, rect.bottom - rect.top);
    m_tilesetView->SetTileset(mapDocument->GetTilesetAssetId());

    m_tilesetView->SetIndicatorColor({251,192,27,255});
    m_tilesetView->SetCursorColor({251,224,147,255});
    m_tilesetView->ShowCursor(false);
    SetSymbolSize({1,1});
}

LRESULT CALLBACK win32_models::SymbolTilesetControl::SymbolTilesetViewStaticProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, [[maybe_unused]]UINT_PTR id, DWORD_PTR data) 
{
    auto *self = reinterpret_cast<SymbolTilesetControl*>(data);

    if (self)
    {
        return self->HandleMessage(
            hwnd,
            msg,
            wparam,
            lparam
        );
    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

sgc::tile::TilePosition2D GetTilePointerPosition(POINT pointerPosition, sgc::tile::TileSize2D tileSize, sgc::graphics::View view)
{
    auto worldPointerPosition = sgc::coordinates::ScreenToWorld(sgc::math::fvec2{
            static_cast<float>(pointerPosition.x),
            static_cast<float>(pointerPosition.y)
        },
            view
    );

    return sgc::tile::TilePosition2D{
        static_cast<sgc::math::ival>(worldPointerPosition.x) / tileSize.x,
        static_cast<sgc::math::ival>(worldPointerPosition.y) / tileSize.y
    };
}

LRESULT win32_models::SymbolTilesetControl::HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    using win32_program::Pointer, win32_program::PointerType, win32_program::PointerBehaviour;

    switch (msg)
    {
        case WM_HSCROLL:
        case WM_VSCROLL:
        {
            const bool isHorizontal = (msg == WM_HSCROLL);
            const int bar = isHorizontal ? SB_HORZ : SB_VERT;

            SCROLLINFO si{};
            si.cbSize = sizeof(si);
            si.fMask = SIF_ALL;

            GetScrollInfo(hwnd, bar, &si);

            int position = si.nPos;

            switch (LOWORD(wparam))
            {
                case SB_LINELEFT:
                    position -= 16;
                    break;

                case SB_LINERIGHT:
                    position += 16;
                    break;

                case SB_PAGELEFT:
                    position -= static_cast<int>(si.nPage);
                    break;

                case SB_PAGERIGHT:
                    position += static_cast<int>(si.nPage);
                    break;

                case SB_THUMBTRACK:
                    position = si.nTrackPos;
                    break;

                case SB_THUMBPOSITION:
                    position = si.nTrackPos;
                    break;

                default:
                    return TRUE;
            }

            const int maxPosition = std::max(
                si.nMin,
                si.nMax - static_cast<int>(si.nPage) + 1
            );

            position = std::clamp(position, si.nMin, maxPosition);

            si.fMask = SIF_POS;
            si.nPos = position;

            SetScrollInfo(hwnd, bar, &si, TRUE);            

            m_tilesetView->SetCameraPositionSingles(
                isHorizontal  ? static_cast<float>(position) : 0,
                !isHorizontal ? static_cast<float>(position) : 0
            );
            Update();
            
            return TRUE;
        }

        case WM_COMMAND:
        {
            auto commandId = LOWORD(wparam);

            switch (commandId)
            {
                case IDOK:
                {
                    EndDialog(hwnd, IDOK);
                    return TRUE;
                }

                case IDCANCEL:
                {
                    EndDialog(hwnd, IDCANCEL);
                    return TRUE;
                }

                default:
                {
                    return TRUE;
                }
            }
            break;
        }

        case WM_POINTERDOWN:
        {
            const auto pointerId = GET_POINTERID_WPARAM(wparam);            

            POINTER_INPUT_TYPE type;
            if (!GetPointerType(pointerId, &type))
                break;

            POINTER_INFO pointerInfo;
            if (!GetPointerInfo(pointerId, &pointerInfo))
                break;

            std::vector<Pointer> pointers;            
            switch (type)
            {
                case PT_TOUCH:
                {
                    pointers = BuildPointersDown(PointerType::Touch, pointerId, pointerInfo.pointerFlags);
                    break;
                }
                    
                case PT_PEN:
                {
                    pointers = BuildPointersDown(PointerType::Pen, pointerId, pointerInfo.pointerFlags);
                    break;
                }
                    
                case PT_MOUSE:
                {
                    pointers = BuildPointersDown(PointerType::Mouse, pointerId, pointerInfo.pointerFlags);
                    break;
                }
                        
                default:
                {
                    break;
                }   
            }

            ScreenToClient(hwnd, &pointerInfo.ptPixelLocation);
            auto tilePosition = GetTilePointerPosition(
                pointerInfo.ptPixelLocation,
                m_tilesetView->GetTileSize(),
                m_tilesetView->GetRenderContext().view
            );
           
            bool needsUpdate = false;

            for(auto &pointer : pointers)
            {
                if(m_pointerManager.CheckPointerBehaviour(pointer, PointerBehaviour::Painting))
                {
                    if(m_tilesetView->Empty() && m_isSelecting.Acquire(pointer))
                    {
                        m_pointerOrigin = tilePosition;                        
                    }
                    else
                    {
                        m_tilesetView->AddTilePosition(tilePosition);                        
                    }

                    needsUpdate = true;
                }
            }

            if(needsUpdate){
                Update();
            }
            
            return TRUE;
        }

        case WM_POINTERUPDATE:
        {
            const auto pointerId = GET_POINTERID_WPARAM(wparam);            

            POINTER_INPUT_TYPE type;
            if (!GetPointerType(pointerId, &type))
                break;

            POINTER_INFO pointerInfo;
            if (!GetPointerInfo(pointerId, &pointerInfo))
                break;

            std::vector<Pointer> pointers;            
            switch (type)
            {
                case PT_TOUCH:
                {
                    pointers = BuildAllPointers(PointerType::Touch, pointerId);
                    break;
                }
                    
                case PT_PEN:
                {
                    pointers = BuildAllPointers(PointerType::Pen, pointerId);
                    break;
                }
                    
                case PT_MOUSE:
                {
                    pointers = BuildAllPointers(PointerType::Mouse, pointerId);
                    break;
                }
                        
                default:
                {
                    break;
                }   
            }
            
            ScreenToClient(hwnd, &pointerInfo.ptPixelLocation);
            auto tilePosition = GetTilePointerPosition(
                pointerInfo.ptPixelLocation,
                m_tilesetView->GetTileSize(),
                m_tilesetView->GetRenderContext().view
            );

            auto previousTilePosition = m_tilesetView->GetCursorPosition();
            bool needsUpdate = false;

            if(!m_tilesetView->IsCursorVisible())
            {
                m_tilesetView->ShowCursor(true);
                needsUpdate = true;
            }            

            if(previousTilePosition != tilePosition)
            {
                for(auto &pointer : pointers)
                {
                    if(m_pointerManager.CheckPointerBehaviour(pointer, PointerBehaviour::Painting))
                    {
                        if(m_isSelecting.IsLockedBy(pointer))
                        {
                            auto sizeInTiles = m_tilesetView->GetSgcTileset().GetSizeInTiles();

                            tilePosition.x = std::min(tilePosition.x, sizeInTiles.x - 1);
                            tilePosition.y = std::min(tilePosition.y, sizeInTiles.y - 1);

                            sgc::math::vec2 minTile = {
                                std::min(m_pointerOrigin.x, tilePosition.x),
                                std::min(m_pointerOrigin.y, tilePosition.y)
                            };

                            sgc::math::vec2 maxTile = {
                                std::max(m_pointerOrigin.x, tilePosition.x),
                                std::max(m_pointerOrigin.y, tilePosition.y)
                            };

                            SetSymbolSize({
                                maxTile.x - minTile.x + 1,
                                maxTile.y - minTile.y + 1
                            });

                            m_tilesetView->SetCursorPosition(m_pointerOrigin);
                        }
                        else
                        {
                            m_tilesetView->SetCursorPosition(tilePosition);
                        }
                        
                        needsUpdate = true;                        
                        break;
                    }
                }
            }

            if(needsUpdate)
            {
                Update();  
            }

            return TRUE;
        }

        case WM_POINTERUP:
        {
            const auto pointerId = GET_POINTERID_WPARAM(wparam);            

            POINTER_INPUT_TYPE type;
            if (!GetPointerType(pointerId, &type))
                break;

            POINTER_INFO pointerInfo;
            if (!GetPointerInfo(pointerId, &pointerInfo))
                break;

            std::vector<Pointer> pointers;            
            switch (type)
            {
                case PT_TOUCH:
                {
                    pointers = BuildPointersUp(PointerType::Touch, pointerId, pointerInfo.pointerFlags);
                    break;
                }
                    
                case PT_PEN:
                {
                    pointers = BuildPointersUp(PointerType::Pen, pointerId, pointerInfo.pointerFlags);
                    break;
                }
                    
                case PT_MOUSE:
                {
                    pointers = BuildPointersUp(PointerType::Mouse, pointerId, pointerInfo.pointerFlags);
                    break;
                }
                        
                default:
                {
                    break;
                }   
            }

            ScreenToClient(hwnd, &pointerInfo.ptPixelLocation);
            auto tilePosition = GetTilePointerPosition(
                pointerInfo.ptPixelLocation,
                m_tilesetView->GetTileSize(),
                m_tilesetView->GetRenderContext().view
            );

            for(auto &pointer : pointers)
            {
                if(m_isSelecting.Release(pointer))
                {
                    m_tilesetView->AddTilePosition(m_pointerOrigin);
                    Update();
                    break;
                }
            }
            
            return TRUE;
        }

        case WM_POINTERLEAVE:
        {
            const auto pointerId = GET_POINTERID_WPARAM(wparam);            

            POINTER_INPUT_TYPE type;
            if (!GetPointerType(pointerId, &type))
                break;

            POINTER_INFO pointerInfo;
            if (!GetPointerInfo(pointerId, &pointerInfo))
                break;

            std::vector<Pointer> pointers;            
            switch (type)
            {
                case PT_TOUCH:
                {
                    pointers = BuildAllPointers(PointerType::Touch, pointerId);
                    break;
                }
                    
                case PT_PEN:
                {
                    pointers = BuildAllPointers(PointerType::Pen, pointerId);
                    break;
                }
                    
                case PT_MOUSE:
                {
                    pointers = BuildAllPointers(PointerType::Mouse, pointerId);
                    break;
                }
                        
                default:
                {
                    break;
                }   
            }

            for(auto &pointer : pointers)
            {
                if(m_pointerManager.CheckPointerBehaviour(pointer, PointerBehaviour::Painting))
                {
                    m_tilesetView->ShowCursor(false);
                    Update();
                    break;
                }
            }

            return TRUE;
        }
    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

void win32_models::SymbolTilesetControl::Update()
{
    m_tilesetView->Render();
}

void win32_models::SymbolTilesetControl::Refresh(program::ProgramContext &programContext)
{
    m_tilesetView->Refresh(programContext);
    
    auto imageSize = m_tilesetView->GetTilesetImageSize();
    win32_helpers::ResizeScrollBarsForImage(
        m_hwnd,
        static_cast<int>(imageSize.x),
        static_cast<int>(imageSize.y)
    );
}

void win32_models::SymbolTilesetControl::SetSymbolSize(sgc::tile::TileSize2D size)
{
    m_tilesetView->SetSizeInTiles(size);
    if(m_sizeChangedCallback){
        m_sizeChangedCallback(size);
    }
}

sgc::tile::TileSize2D win32_models::SymbolTilesetControl::GetSymbolSize() const
{
    return m_tilesetView->GetSymbolSize();
}

sgc::tile::TileSize2D win32_models::SymbolTilesetControl::GetMaxSymbolSize() const
{
    return m_tilesetView->GetMaxSymbolSize();
}

void win32_models::SymbolTilesetControl::RegisterOnSizeChangedCallback(std::function<void(sgc::tile::TileSize2D)> sizeChangedCallback)
{
    m_sizeChangedCallback = sizeChangedCallback;
}

void win32_models::SymbolTilesetControl::ResetCurrentView()
{
    m_tilesetView->ClearSymbolPositions();
}
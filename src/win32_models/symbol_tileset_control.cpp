#include "win32_models/symbol_tileset_control.hpp"
#include "win32_helpers/resize_helpers.hpp"
#include "program/program.hpp"
#include <sgc/coordinates/screenworld.hpp>

bool win32_models::SymbolTilesetControl::m_classRegistered = false;
const wchar_t* g_className = L"SymbolTilesetControl";

win32_models::SymbolTilesetControl::SymbolTilesetControl(program::ProgramContext &programContext)
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
    m_tilesetView->SetCursorColor({251,192,27,255});
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

LRESULT win32_models::SymbolTilesetControl::HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
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

        case WM_POINTERUPDATE:
        {            
            return TRUE;
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

            ScreenToClient(hwnd, &pointerInfo.ptPixelLocation);
            
            auto pointerPosition = sgc::graphics::PixelPosition2D{
                pointerInfo.ptPixelLocation.x,
                pointerInfo.ptPixelLocation.y
            };

            auto position = sgc::coordinates::ScreenToWorld(sgc::math::fvec2{
                    static_cast<float>(pointerPosition.x),
                    static_cast<float>(pointerPosition.y)
                },
                    m_tilesetView->GetRenderContext().view
            );

            auto tileSize = m_tilesetView->GetTileSize();
            m_tilesetView->AddTilePosition({
                static_cast<sgc::math::ival>(position.x) / tileSize.x,
                static_cast<sgc::math::ival>(position.y) / tileSize.y
            });
            m_tilesetView->SetSizeInTiles({1,1});
            
            Update();
            break;
        }

        case WM_POINTERUP:
        {
            break;
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
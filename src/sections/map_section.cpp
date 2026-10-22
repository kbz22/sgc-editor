#include "sections/map_section.hpp"
#include "program/program.hpp"
#include "command/paint_command.hpp"
#include "command/command_manager.hpp"
#include "command/paint_selection_command.hpp"

#include <sgc/coordinates/screenworld.hpp>
#include <sgc/data/statictilestorage.hpp>
#include <sgc/sdl/sdl_win32.hpp>
#include <fstream>
#include <filesystem>
#include <windowsx.h>
#include <commctrl.h>
#include <cmath>

constexpr int gc_SelectionRectangleTimerId = 1;
constexpr int gc_TemporalToolTimerId = 2;

sections::MapSection::MapSection(program::ProgramContext& programContext) :
    Section{
        L"MapView",
        win32_program::ControlId::MapView,
        programContext.GetMainWindowHandle(),
        programContext.GetHInstance()
    },
    m_mapView{std::make_unique<sgc_view::MapView>(GetHwnd())},
    m_editorToolManager{
        *m_mapView,
        *this,
        *programContext.GetSection<sections::TilesetSection>()
    }
{
    AttachView(*m_mapView);

    m_mapView->RegisterOnCursorPositionChangedCallback([this](sgc::math::vec2 position) 
    {
        auto &programContext = program::GetProgramContext();
        auto statusSection = programContext.GetSection<sections::StatusSection>();
        auto mapDocument = programContext.GetManager<file::FileManager>()->GetActiveDocument();
        auto tileSize = m_mapView->GetTileSize();
        auto layers = mapDocument->GetLayerManager()->GetLayers();

        auto x = position.x / tileSize.x;
        auto y = position.y / tileSize.y;

        if(layers.empty()) {
            statusSection->SetStatusCursorPosition({x,y});
            statusSection->SetStatusTileId(std::nullopt);
            return;
        }
        else
        {
            auto currentLayer = layers[mapDocument->GetLayerManager()->GetActiveLayerIndex()];
            auto &sgcTileset =  m_mapView->m_sgcTileset;
            auto tileId = currentLayer.storage->GetTileAt({x,y});

            statusSection->SetStatusCursorPosition({x,y});
            if(tileId.has_value() && sgcTileset.IsSpecial(tileId.value())){
                statusSection->SetStatusTileId(sgcTileset.GetSpecialTileName(tileId.value()));
            }
            else{
                statusSection->SetStatusTileId(tileId);
            }
        }
    });

    m_mapView->RegisterOnSelectionSizeChangedCallback([this](sgc::math::vec2 size) 
    {
        auto &programContext = program::GetProgramContext();
        auto statusSection = programContext.GetSection<sections::StatusSection>();
        auto tileSize = m_mapView->GetTileSize();

        statusSection->SetStatusSelectionSize({
            size.x / tileSize.x,
            size.y / tileSize.y
        });
    });

    SetTimer(
        GetHwnd(),
        gc_SelectionRectangleTimerId,
        static_cast<int>(m_selectionRectUpdateInterval.count()),
        nullptr
    );

    m_editorToolManager.SetModeDelay(
        editor_tools::PaintMode::Scatter,
        m_editorToolsUpdateInterval
    );    
    SetTimer(
        GetHwnd(),
        gc_TemporalToolTimerId,
        static_cast<int>(m_editorToolsUpdateInterval.count()),
        nullptr
    );

    m_mapView->Render();    
}
 
void sections::MapSection::Update()
{
    auto currentTime = std::chrono::steady_clock::now();
    if (currentTime - m_lastUpdateTime >= m_timeBetweenUpdates) 
    {
        m_lastUpdateTime = currentTime;

        if (m_mapView != nullptr) {
            m_mapView->Render();
        }
    }
}

void sections::MapSection::Refresh(program::ProgramContext& programContext)
{    
    if (m_mapView != nullptr) {
        m_mapView->Refresh(programContext);
    }
}

void sections::MapSection::HandleSectionResize()
{
    if (m_mapView != nullptr) {
        RECT rect;
        GetClientRect(GetHwnd(), &rect);
        m_mapView->SetScreenSize(rect.right - rect.left, rect.bottom - rect.top);        
    }
    Update();
}

LRESULT sections::MapSection::HandleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{ 
    program::ProgramContext& programContext = program::GetProgramContext();
    auto mapDocument = programContext.GetManager<file::FileManager>()->GetActiveDocument();

    if(mapDocument == nullptr || !mapDocument->IsEditable())
    {
        return DefSubclassProc(hwnd, msg, wparam, lparam);
    }

    auto SetCaptureHelper = [this](HWND hwnd, Pointer pointer) {
        if (!m_isCaptured.IsLocked()) {
            SetCapture(hwnd);
            m_isCaptured.Acquire(pointer);
        }
    };

    auto ReleaseCaptureHelper = [this](Pointer pointer) {
        if (m_isCaptured.IsLockedBy(pointer)) {
            ReleaseCapture();
            m_isCaptured.Release(pointer);
        }
    };

    switch (msg)
    {
        case WM_POINTERUPDATE:
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
            
            switch (type)
            {
                case PT_TOUCH:
                {                    
                    PointerUpdate({PointerType::Touch, pointerId}, pointerPosition, programContext);
                    break;
                }
                    
                case PT_PEN:
                {
                    PointerUpdate({PointerType::Pen, pointerId}, pointerPosition, programContext);
                    break;
                }
                    
                case PT_MOUSE:
                {
                    break;
                }
                        
                default:
                {
                    break;
                }   
            }

            return 0;
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
            
            switch (type)
            {
                case PT_TOUCH:
                {
                    PointerDown({PointerType::Touch, pointerId}, pointerPosition, programContext);                    
                    break;
                }
                    
                case PT_PEN:
                {
                    PointerDown({PointerType::Pen, pointerId}, pointerPosition, programContext);
                    break;
                }
                    
                case PT_MOUSE:
                {
                    break;
                }
                        
                default:
                {
                    break;
                }   
            }

            return 0;
        }

        case WM_POINTERUP:
        {
            const auto pointerId = GET_POINTERID_WPARAM(wparam);

            POINTER_INPUT_TYPE type;
            if (!GetPointerType(pointerId, &type))
                break;

            POINTER_INFO pointerInfo;
            if (GetPointerInfo(pointerId, &pointerInfo))                
            switch (type)
            {
                case PT_TOUCH:
                {
                    PointerUp({PointerType::Touch, pointerId}, mapDocument, programContext);
                    break;
                }
                    
                case PT_PEN:
                {
                    PointerUp({PointerType::Pen, pointerId}, mapDocument, programContext);
                    break;
                }
                    
                case PT_MOUSE:
                {
                    break;
                }
                        
                default:
                {
                    break;
                }   
            }

            return 0;
        }        

        case WM_MOUSEMOVE:
        {
            bool noPointerUpdate = true;

            auto x = static_cast<sgc::math::ival>(GET_X_LPARAM(lparam));
            auto y = static_cast<sgc::math::ival>(GET_Y_LPARAM(lparam));            

            if ((wparam & MK_LBUTTON))
            {
               PointerUpdate({PointerType::LeftMouse, 0}, {x,y}, programContext);
               noPointerUpdate = false;
            }

            if (wparam & MK_RBUTTON)
            {
                PointerUpdate({PointerType::RightMouse, 0}, {x,y}, programContext);
                noPointerUpdate = false;
            }

            if (wparam & MK_MBUTTON)
            {
                PointerUpdate({PointerType::MiddleMouse, 0}, {x,y}, programContext);
                noPointerUpdate = false;
            }

            if(noPointerUpdate) {                
                if(UpdateCursorPosition({x,y}, programContext)){
                    Update();
                }
            }

            return 0;
        }

        case WM_LBUTTONUP:
        {
            PointerUp({PointerType::LeftMouse, 0}, mapDocument, programContext);
            ReleaseCaptureHelper({PointerType::LeftMouse, 0});
            return 0;
        }

        case WM_MBUTTONUP:
        {
            PointerUp({PointerType::MiddleMouse, 0}, mapDocument, programContext);
            ReleaseCaptureHelper({PointerType::MiddleMouse, 0});
            return 0;
        }

        case WM_RBUTTONUP:
        {
            PointerUp({PointerType::RightMouse, 0}, mapDocument, programContext);
            ReleaseCaptureHelper({PointerType::RightMouse, 0});
            return 0;
        }

        case WM_LBUTTONDOWN:
        {
            auto x = static_cast<sgc::math::ival>(GET_X_LPARAM(lparam));
            auto y = static_cast<sgc::math::ival>(GET_Y_LPARAM(lparam));

            auto screenPosition = sgc::math::vec2{
                x,
                y
            };

            PointerDown({PointerType::LeftMouse, 0}, screenPosition, programContext);
            SetCaptureHelper(hwnd, {PointerType::LeftMouse, 0});
            return 0;
        }

        case WM_MBUTTONDOWN:
        {
            auto x = static_cast<sgc::math::ival>(GET_X_LPARAM(lparam));
            auto y = static_cast<sgc::math::ival>(GET_Y_LPARAM(lparam));

            auto screenPosition = sgc::math::vec2{
                x,
                y
            };

            PointerDown({PointerType::MiddleMouse, 0}, screenPosition, programContext);
            SetCaptureHelper(hwnd, {PointerType::MiddleMouse, 0});
            return 0;
        }

        case WM_RBUTTONDOWN:
        {
            auto x = static_cast<sgc::math::ival>(GET_X_LPARAM(lparam));
            auto y = static_cast<sgc::math::ival>(GET_Y_LPARAM(lparam));

            auto screenPosition = sgc::math::vec2{
                x,
                y
            };

            PointerDown({PointerType::RightMouse, 0}, screenPosition, programContext);
            SetCaptureHelper(hwnd, {PointerType::RightMouse, 0});
            return 0;
        }

        case WM_MOUSEWHEEL:
        {
            auto delta = GET_WHEEL_DELTA_WPARAM(wparam);

            if (delta != 0)
            {
                POINT point{
                    GET_X_LPARAM(lparam),
                    GET_Y_LPARAM(lparam)
                };

                ScreenToClient(hwnd, &point);

                auto anchor = sgc::math::fvec2{
                    static_cast<float>(point.x),
                    static_cast<float>(point.y)
                };

                auto zoomFactor = 1.1f;

                if (delta < 0)
                {
                    zoomFactor = 1.0f / zoomFactor;
                }

                ExecuteZoom(
                    anchor,
                    m_mapView->GetZoom() * zoomFactor
                );
            }

            break;
        }

        case WM_TIMER:
        {
            switch(wparam)
            {
                case gc_SelectionRectangleTimerId:
                {
                    if(m_editorToolManager.GetPaintMode() == editor_tools::PaintMode::Select)
                    {
                        m_mapView->UpdateSelectionOffset();
                        Update();
                    }
                    break;
                }
                
                case gc_TemporalToolTimerId:
                {                    
                    POINT pos{};

                    if(!m_isPainting.IsLocked())
                        return TRUE;

                    if(IsMouse(m_isPainting.GetPointerType()))
                    {
                        if (!GetCursorPos(&pos))
                            return {};

                        ScreenToClient(hwnd, &pos);                        
                    }
                    else
                    {
                        POINTER_INFO info{};

                        if (!GetPointerInfo(m_isPainting.GetPointerId(), &info))
                            return {};

                        pos = info.ptPixelLocation;
                        ScreenToClient(hwnd, &pos);
                    }

                    if(m_editorToolManager.NeedsUpdatesPeriodically() && m_isPainting.IsLocked() && GetCursorPos(&pos) && ScreenToClient(hwnd, &pos))
                    {
                        auto tileSize = m_mapView->GetTileSize();                        
                        m_editorToolManager.Execute(
                            *mapDocument,
                            sgc::tile::TilePosition2D{
                                pos.x / tileSize.x,
                                pos.y / tileSize.y
                            }
                        );
                        Update();
                    }
                    break;
                }
            }
            

            break;
        }
    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

void sections::MapSection::SetAllowChunkCreation(bool check)
{
    m_editorToolManager.SetAllowChunkCreation(check);
}

void sections::MapSection::SetPaintMode(editor_tools::PaintMode paintMode)
{
    switch(paintMode)
    {
        case editor_tools::PaintMode::ChunkRemover:
            m_mapView->SetCursorMode(sgc_view::CursorTileMode::RemoveChunk);
            break;

        default:
            m_mapView->SetCursorMode(sgc_view::CursorTileMode::Default);
            break;
    }

    m_editorToolManager.SetPaintMode(paintMode);

    Update();
}

void sections::MapSection::SetSelectionMode(editor_tools::SelectionMode selectionMode)
{
    m_editorToolManager.SetSelectionMode(selectionMode);
}

editor_tools::PaintMode sections::MapSection::GetPaintMode() const
{
    return m_editorToolManager.GetPaintMode();
}

editor_tools::SelectionMode sections::MapSection::GetSelectionMode() const
{
    return m_editorToolManager.GetSelectionMode();
}

void sections::MapSection::SetSelectionMoveMode(bool canMoveSelection)
{
    m_canMoveSelection = canMoveSelection;
}

void sections::MapSection::SetSelectionPositionInTiles(sgc::tile::TilePosition2D position)
{
    auto tileSize = m_mapView->GetTileSize();
    m_mapView->SetSelectionPositionInPixels({
        static_cast<sgc::math::ival>(position.x * tileSize.x),
        static_cast<sgc::math::ival>(position.y * tileSize.y)
    });
}

void sections::MapSection::SetSelectionSizeInTiles(sgc::tile::TileSize2D size)
{
    auto tileSize = m_mapView->GetTileSize();
    m_mapView->SetSelectionSizeInPixels({
        static_cast<sgc::math::ival>(size.x * tileSize.x),
        static_cast<sgc::math::ival>(size.y * tileSize.y)
    });
}

float sections::MapSection::GetZoom() const
{
    if (m_mapView != nullptr) {
        return m_mapView->GetZoom();
    }
    return 1.0f;
}

sgc::math::fvec2 sections::MapSection::GetScreenCenterWorldPosition() const
{
    if (m_mapView != nullptr) {
        auto renderContext = m_mapView->GetRenderContext();

        auto centerScreenPos = sgc::math::fvec2{
            static_cast<float>(renderContext.view.screen.w) / 2.0f,
            static_cast<float>(renderContext.view.screen.h) / 2.0f
        };
        
        auto view = m_mapView->GetView();
        return sgc::coordinates::ScreenToWorld(centerScreenPos, view);
    }
    return sgc::math::fvec2{0.0f, 0.0f};
}

void sections::MapSection::ExecuteZoom(sgc::math::fvec2 anchorPoint, float zoomValue)
{
    auto view = m_mapView->GetView();

    auto worldPosBeforeZoom =
        sgc::coordinates::ScreenToWorld(anchorPoint, view);

    if(!m_mapView->SetZoom(zoomValue))
    {
        return;
    }

    view = m_mapView->GetView();

    auto worldPosAfterZoom =
        sgc::coordinates::ScreenToWorld(anchorPoint, view);

    auto cameraDelta =
        worldPosBeforeZoom - worldPosAfterZoom;

    cameraDelta *= -1.0f;

    m_mapView->ChangeCameraPositionSingles(
        cameraDelta.x,
        cameraDelta.y
    );   
    
    m_onZoomChangedCallback(zoomValue);

    Update();
}

void sections::MapSection::RegisterOnZoomChangedCallback(std::function<void(float)> callback)
{
    m_onZoomChangedCallback = callback;
}

bool sections::MapSection::IsSelectionActive() const
{
    if (m_mapView != nullptr) {
        auto selectionSize = m_mapView->GetSelectionSizeInTiles();
        return selectionSize.x > 0 && selectionSize.y > 0;
    }
    return false;
}

void sections::MapSection::ResetSelection()
{    
    m_mapView->SetSelectionSizeInPixels({0, 0});
    Update();
}

sgc::tile::TilePosition2D sections::MapSection::GetSelectionRectanglePositionTiles() const
{
    if (m_mapView != nullptr) {
        auto selectionPos = m_mapView->GetSelectionPositionInTiles();
        auto tileSize = m_mapView->GetTileSize();

        return sgc::tile::TilePosition2D{
            static_cast<sgc::math::ival>(selectionPos.x),
            static_cast<sgc::math::ival>(selectionPos.y)
        };
    }
    return sgc::tile::TilePosition2D{0, 0};
}

sgc::tile::TileSize2D sections::MapSection::GetSelectionRectangleSizeTiles() const
{
    if (m_mapView != nullptr) {
        auto selectionSize = m_mapView->GetSelectionSizeInTiles();

        return sgc::tile::TileSize2D{
            static_cast<sgc::math::ival>(selectionSize.x),
            static_cast<sgc::math::ival>(selectionSize.y)
        };
    }
    return sgc::tile::TileSize2D{0, 0};
}

sgc::tile::TilePosition2D sections::MapSection::GetCursorPositionInTiles() const
{
    return m_mapView->GetCursorPositionInTiles();
}

bool sections::MapSection::GetSelectionMoveMode() const
{
    return m_canMoveSelection;
}

void sections::MapSection::RenderToImage(std::filesystem::path outputPath)
{
    if (m_mapView != nullptr) {
        m_mapView->RenderToImage(outputPath);
    }
}

bool sections::MapSection::UpdateCursorPosition(sgc::graphics::PixelPosition2D pointerPosition, program::ProgramContext& programContext)
{    
    auto screenPosition = sgc::math::fvec2{
        static_cast<float>(pointerPosition.x),
        static_cast<float>(pointerPosition.y)
    };

    auto view = m_mapView->GetView();
    auto worldPosition = sgc::coordinates::ScreenToWorld(screenPosition, view);

    auto tileSize = m_mapView->GetTileSize();
    auto x_tile = static_cast<sgc::math::ival>(
        std::floor(worldPosition.x / tileSize.x) * tileSize.x
    );

    auto y_tile = static_cast<sgc::math::ival>(
        std::floor(worldPosition.y / tileSize.y) * tileSize.y
    );

    auto position = m_mapView->GetCursorPositionInTiles();
    if (position.x != x_tile || position.y != y_tile) 
    {        
        // auto selection = programContext.GetSection<sections::TilesetSection>()->GetCursorSizeInPixels().value_or(sgc::graphics::PixelSize2D{tileSize.x, tileSize.y});
        auto selection = programContext.GetSection<sections::TilesetSection>()->GetCursorSizeInPixels();

        switch(m_editorToolManager.GetPaintMode())
        {
            case editor_tools::PaintMode::Rectangle:
            case editor_tools::PaintMode::ChunkRemover:
            case editor_tools::PaintMode::Select:
            case editor_tools::PaintMode::MoveSelection:
            case editor_tools::PaintMode::TilePicker:
                m_mapView->SetCursorSizeInPixels({
                    tileSize.x,
                    tileSize.y
                });
                break;

            default:
                m_mapView->SetCursorSizeInPixels({
                    selection.x,
                    selection.y
                });
                break;
        }                

        m_mapView->SetCursorPositionInPixels({
            x_tile,
            y_tile
        });        

        return true;
    }

    return false;
}

bool sections::MapSection::UpdateSelectionMove(Pointer pointer, file::MapDocument *mapDocument, sgc::graphics::PixelPosition2D pointerPosition)
{
    if(m_isMovingSelection.IsLockedBy(pointer))
    {
        auto screenPosition = sgc::math::fvec2{
            static_cast<float>(pointerPosition.x),
            static_cast<float>(pointerPosition.y)
        };

        auto view = m_mapView->GetView();
        auto worldPosition = sgc::coordinates::ScreenToWorld(screenPosition, view);

        auto tileSize = m_mapView->GetTileSize();
        auto x_tile = static_cast<sgc::math::ival>(
            std::floor(worldPosition.x / tileSize.x)
        );

        auto y_tile = static_cast<sgc::math::ival>(
            std::floor(worldPosition.y / tileSize.y)
        );

        m_editorToolManager.Execute(*mapDocument, {x_tile, y_tile});
        return true;
    }
    
    return false;
}

bool sections::MapSection::UpdateDragDrawing(Pointer pointer, program::ProgramContext& programContext)
{
    auto mapDocument = programContext.GetManager<file::FileManager>()->GetActiveDocument();
    auto tilesetSection = programContext.GetSection<TilesetSection>();
    auto tileSize = m_mapView->GetTileSize();
    auto selectionSize = tilesetSection->GetCursorSizeInPixels();
    auto cursorTileSize = sgc::tile::TileSize2D{
        static_cast<sgc::math::ival>(selectionSize.x / tileSize.x),
        static_cast<sgc::math::ival>(selectionSize.y / tileSize.y)
    };

    auto cursorTilePositionOnTileset = m_mapView->PixelsToTiles(                
        tilesetSection->GetCursorPositionInPixels()
    );

    auto cursorPositionOnMap = m_mapView->GetCursorPositionInTiles();

    if(m_isPainting.IsLockedBy(pointer)) {
        m_editorToolManager.Execute(*mapDocument, cursorPositionOnMap);

        return true;
    }
    return false;
}

bool sections::MapSection::UpdateOnCursorDown(Pointer pointer)
{
    auto cursorPositionOnMap = m_mapView->GetCursorPositionInTiles();
    auto tileSize = m_mapView->GetTileSize();

    if(m_canMoveSelection && !m_isMovingSelection.IsLocked())
    {
        auto selectionPos = m_mapView->GetSelectionPositionInTiles();
        auto selectionSize = m_mapView->GetSelectionSizeInTiles();

        if(cursorPositionOnMap.x >= selectionPos.x && cursorPositionOnMap.x <= selectionPos.x + selectionSize.x &&
            cursorPositionOnMap.y >= selectionPos.y && cursorPositionOnMap.y <= selectionPos.y + selectionSize.y)
        {                    
            m_isMovingSelection.Acquire(pointer);            
            m_editorToolManager.SetPaintMode(editor_tools::PaintMode::MoveSelection);
        }
        else
        {
            m_editorToolManager.SetPaintMode(editor_tools::PaintMode::Select);            
        }

        m_isPainting.Acquire(pointer);

        return true;
    }

    if(!m_isPainting.IsLocked()) {
        m_isPainting.Acquire(pointer);
    }

    if(m_editorToolManager.NeedsRedraw()) {
        return true;
    }
    
    return false;
}

bool sections::MapSection::UpdateOnCursorUp(Pointer pointer, file::MapDocument *mapDocument, program::ProgramContext& programContext)
{
    auto returnFlag = false;    

    if(m_isPainting.Release(pointer)) 
    {
        m_editorToolManager.Commit(*mapDocument);
        returnFlag = true;
    }

    if(m_isMovingSelection.Release(pointer)) 
    {        
        m_editorToolManager.SetPaintMode(editor_tools::PaintMode::Select);        
        returnFlag = true;
    }
    
    programContext.UpdateCanUndoRedo();

    return returnFlag;
}

void sections::MapSection::PointerUpdate(Pointer pointer, sgc::graphics::PixelPosition2D pointerPosition, program::ProgramContext& programContext)
{    
    bool shouldUpdate = false;

    if(IsPointerPanning(pointer.Type))
    {
        shouldUpdate = PanningUpdate(pointer, pointerPosition);
    }    
    else if(IsPointerPainting(pointer.Type))
    {
        auto result = UpdateCursorPosition(pointerPosition, programContext);
        shouldUpdate = result;
        result = UpdateSelectionMove(pointer, programContext.GetManager<file::FileManager>()->GetActiveDocument(), pointerPosition);
        shouldUpdate = shouldUpdate || result;
        result = UpdateDragDrawing(pointer, programContext);
        shouldUpdate = shouldUpdate || result;
    }
    else if(IsMouse(pointer.Type))
    {
        shouldUpdate = UpdateCursorPosition(pointerPosition, programContext);
    }

    if(shouldUpdate) {
        Update();
    }
}

bool sections::PointerLock::IsLocked() const {
    return m_isLocked;
}

bool sections::PointerLock::IsLockedBy(Pointer pointer) const {
    return m_isLocked && m_pointer == pointer;
}

bool sections::PointerLock::Acquire(Pointer pointer) {
    if(m_isLocked) {
        return false;
    }

    m_pointer = pointer;
    m_isLocked = true;
    return true;
}

bool sections::PointerLock::Release(Pointer pointer) {
    if(IsLockedBy(pointer)) {
        m_isLocked = false;
        return true;
    }
    return false;
}

sections::PointerType sections::PointerLock::GetPointerType() const
{
    return m_pointer.Type;
}

uint32_t sections::PointerLock::GetPointerId() const
{
    return m_pointer.Id;
}

void sections::MapSection::PanningDown(Pointer pointer, sgc::graphics::PixelPosition2D position)
{
    if(m_isPanning.IsLocked())
        return;

    m_isPanning.Acquire(pointer);

    m_lastMousePosPan.x = position.x;
    m_lastMousePosPan.y = position.y;
}

void sections::MapSection::PanningUp(Pointer pointer)
{    
    m_isPanning.Release(pointer);
}

bool sections::MapSection::PanningUpdate(Pointer pointer, sgc::graphics::PixelPosition2D position)
{
    if(!m_isPanning.IsLockedBy(pointer))
        return false;
    
    auto deltaX = m_mapView->ScaleForZoom(static_cast<float>(position.x - m_lastMousePosPan.x));
    auto deltaY = m_mapView->ScaleForZoom(static_cast<float>(position.y - m_lastMousePosPan.y));

    m_mapView->ChangeCameraPositionSingles(
        deltaX,
        deltaY
    );

    m_lastMousePosPan.x = position.x;
    m_lastMousePosPan.y = position.y;

    return true;
}

void sections::MapSection::SetPointerBehaviour(PointerType pointerType, PointerBehaviour behaviour)
{
    if(behaviour == PointerBehaviour::None)
    {
        m_pointerBehaviour.erase(pointerType);
    }
    else
    {
        m_pointerBehaviour[pointerType] = behaviour;
    }
}

void sections::MapSection::PointerDown(Pointer pointer, sgc::graphics::PixelPosition2D position, program::ProgramContext& programContext)
{
    if(IsPointerPanning(pointer.Type))
    {
        PanningDown(pointer, position);
    }
    else if(IsPointerPainting(pointer.Type))
    {
        bool shouldUpdate = UpdateOnCursorDown(pointer);
        PointerUpdate(pointer, position, programContext);

        if(m_editorToolManager.NeedsRedraw() || shouldUpdate) {
            Update();
        }
    }
}

void sections::MapSection::PointerUp(Pointer pointer, file::MapDocument *mapDocument, program::ProgramContext& programContext)
{
    if(m_isPanning.IsLockedBy(pointer))
    {
        PanningUp(pointer);
    }

    if(m_isPainting.IsLockedBy(pointer))
    {
        UpdateOnCursorUp(pointer, mapDocument, programContext);
    }
}

//! for now the pointer painting/panning is decided by type alone. 
//  With the new changes it would make sense to make it id dependent too but 
//  I don't think it's a good idea unless mouse is also passed via the pointer API
//  and that's a bigger refactor of every section which I don't feel like doing right now

bool sections::MapSection::IsPointerPainting(PointerType pointerType)
{
    return m_pointerBehaviour.contains(pointerType) && m_pointerBehaviour[pointerType] == PointerBehaviour::Painting;
}

bool sections::MapSection::IsPointerPanning(PointerType pointerType)
{
    return m_pointerBehaviour.contains(pointerType) && m_pointerBehaviour[pointerType] == PointerBehaviour::Panning;
}

void sections::MapSection::SetSelectionMovedImage(program::SelectionMovedStorage &storageLayers)
{
    for(auto &[storageIndex, storageLayer] : storageLayers)
    {
        auto selectionImage = std::make_shared<sgc::graphics::TiledImage>(
            std::make_shared<sgc::graphics::TiledLayer>(
                m_mapView->m_sgcTileset.GetTileset(),
                storageLayer.storage
            )
        );
        selectionImage->SetAlpha(storageLayer.transparency);

        auto selectionLayers = m_mapView->GetSelectionLayers();
        selectionLayers->Set(storageLayer.renderingIndex, selectionImage);
    }
}

void sections::MapSection::UpdateSelectionMovedPosition(sgc::tile::TilePosition2D positionOnMap)
{
    auto selectionLayers = m_mapView->GetSelectionLayers();
    auto tileSize = m_mapView->GetTileSize();

    for(auto &[layerIndex, layer] : *selectionLayers) 
    {
        auto selectionImage = std::dynamic_pointer_cast<sgc::graphics::TiledImage>(layer);        

        if(selectionImage) {
            selectionImage->SetPositionPixels(sgc::math::vec2{
                positionOnMap.x * tileSize.x,
                positionOnMap.y * tileSize.y
            });
        }
    }

    m_mapView->SetSelectionPositionInPixels({
        positionOnMap.x * tileSize.x,
        positionOnMap.y * tileSize.y
    });
}
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

constexpr int gc_TimerId = 1;

sections::MapSection::MapSection(program::ProgramContext& programContext) :
    Section{L"MapView", win32_program::ControlId::MapView, programContext.GetMainWindowHandle(), programContext.GetHInstance()},
    m_mapView{std::make_unique<sgc_view::MapView>(GetHwnd())},
    m_brush{programContext.GetSelectionRectangleOnTileset()}
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
            auto tileId = currentLayer.storage->GetTileAt({x,y});

            statusSection->SetStatusCursorPosition({x,y});
            if(tileId == m_mapView->m_tileset->TileIdCount()) {
                statusSection->SetStatusTileId(L"Empty");
            } else {
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
        gc_TimerId,
        static_cast<int>(m_selectionRectUpdateInterval.count()),
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

    auto SetCaptureHelper = [this](HWND hwnd, PointerType pointerType) {
        if (!m_isCaptured.IsLocked()) {
            SetCapture(hwnd);
            m_isCaptured.Acquire(pointerType);
        }
    };

    auto ReleaseCaptureHelper = [this](PointerType pointerType) {
        if (m_isCaptured.IsLockedBy(pointerType)) {
            ReleaseCapture();
            m_isCaptured.Release(pointerType);
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
            ScreenToClient(hwnd, &pointerInfo.ptPixelLocation);
            auto pointerPosition = sgc::graphics::PixelPosition2D{
                pointerInfo.ptPixelLocation.x,
                pointerInfo.ptPixelLocation.y
            };

            if (GetPointerInfo(pointerId, &pointerInfo))                
            switch (type)
            {
                case PT_TOUCH:
                {                    
                    PointerUpdate(PointerType::Touch, pointerPosition, programContext);
                    break;
                }
                    
                case PT_PEN:
                {
                    PointerUpdate(PointerType::Pen, pointerPosition, programContext);
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

            ScreenToClient(hwnd, &pointerInfo.ptPixelLocation);
            auto pointerPosition = sgc::graphics::PixelPosition2D{
                pointerInfo.ptPixelLocation.x,
                pointerInfo.ptPixelLocation.y
            };

            if (GetPointerInfo(pointerId, &pointerInfo))
            switch (type)
            {
                case PT_TOUCH:
                {
                    PointerDown(PointerType::Touch, mapDocument, pointerPosition, programContext);                    
                    break;
                }
                    
                case PT_PEN:
                {
                    PointerDown(PointerType::Pen, mapDocument, pointerPosition, programContext);
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
                    PointerUp(PointerType::Touch, mapDocument, programContext);
                    break;
                }
                    
                case PT_PEN:
                {
                    PointerUp(PointerType::Pen, mapDocument, programContext);
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
               PointerUpdate(PointerType::LeftMouse, {x,y}, programContext);
               noPointerUpdate = false;
            }

            if (wparam & MK_RBUTTON)
            {
                PointerUpdate(PointerType::RightMouse, {x,y}, programContext);
                noPointerUpdate = false;
            }

            if (wparam & MK_MBUTTON)
            {
                PointerUpdate(PointerType::MiddleMouse, {x,y}, programContext);
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
            PointerUp(PointerType::LeftMouse, mapDocument, programContext);
            ReleaseCaptureHelper(PointerType::LeftMouse);
            return 0;
        }

        case WM_MBUTTONUP:
        {
            PointerUp(PointerType::MiddleMouse, mapDocument, programContext);
            ReleaseCaptureHelper(PointerType::MiddleMouse);
            return 0;
        }

        case WM_RBUTTONUP:
        {
            PointerUp(PointerType::RightMouse, mapDocument, programContext);
            ReleaseCaptureHelper(PointerType::RightMouse);
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

            PointerDown(PointerType::LeftMouse, mapDocument, screenPosition, programContext);
            SetCaptureHelper(hwnd, PointerType::LeftMouse);
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

            PointerDown(PointerType::MiddleMouse, mapDocument, screenPosition, programContext);
            SetCaptureHelper(hwnd, PointerType::MiddleMouse);
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

            PointerDown(PointerType::RightMouse, mapDocument, screenPosition, programContext);
            SetCaptureHelper(hwnd, PointerType::RightMouse);
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
            if (wparam == gc_TimerId && m_brush.GetPaintMode() == editor_tools::PaintMode::Select)
            {
                m_mapView->UpdateSelectionOffset();
                Update();                
            }
            break;
        }
    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

void sections::MapSection::SetCheckTileBeforePainting(bool check)
{
    m_brush.SetCheckTileBeforePainting(check);
}

void sections::MapSection::SetPaintMode(editor_tools::PaintMode paintMode)
{
    m_brush.SetPaintMode(paintMode);    
}

void sections::MapSection::SetEraseMode(editor_tools::EraserMode eraserMode)
{
    m_brush.SetEraserMode(eraserMode);

    switch(m_brush.GetEraserMode())
    {
        case editor_tools::EraserMode::None:
        {
            m_mapView->SetCursorMode(sgc_view::CursorTileMode::Default);
            break;
        }

        case editor_tools::EraserMode::DeleteChunk:
        {
            m_mapView->SetCursorMode(sgc_view::CursorTileMode::RemoveChunk);
            break;
        }

        case editor_tools::EraserMode::ClearTile:
        {
            m_mapView->SetCursorMode(sgc_view::CursorTileMode::RemoveTile);
            break;
        }
    }

    Update();
}

void sections::MapSection::SetSelectionMode(editor_tools::SelectionMode selectionMode)
{
    m_brush.SetSelectionMode(selectionMode);
}

editor_tools::PaintMode sections::MapSection::GetPaintMode() const
{
    return m_brush.GetPaintMode();
}

editor_tools::EraserMode sections::MapSection::GetEraseMode() const
{
    return m_brush.GetEraserMode();
}

editor_tools::SelectionMode sections::MapSection::GetSelectionMode() const
{
    return m_brush.GetSelectionMode();
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
    if (position.x != x_tile || position.y != y_tile) {                
        
        auto selection = programContext.GetSection<sections::TilesetSection>()->GetCursorSizeInPixels().value_or(sgc::graphics::PixelSize2D{tileSize.x, tileSize.y});

        switch(m_brush.GetPaintMode())
        {                    
            case editor_tools::PaintMode::Rectangle:
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

bool sections::MapSection::UpdateSelectionMove(PointerType pointerType, sgc::graphics::PixelPosition2D pointerPosition)
{
    if(m_isMovingSelection.IsLockedBy(pointerType))
    {
        auto selectionLayers = m_mapView->GetSelectionLayers();
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

        m_mapView->SetSelectionPositionInPixels({
            x_tile - m_movingSelectionOffset.x * tileSize.x,
            y_tile - m_movingSelectionOffset.y * tileSize.y
        });

        //! I think I need a better way to handle position in the lib, but this will do for now
        for(auto &[layerIndex, layer] : *selectionLayers) {
            auto selectionImage = std::dynamic_pointer_cast<sgc::graphics::TiledImage>(layer);
            if(selectionImage) {
                selectionImage->SetPositionPixels(sgc::math::vec2{
                    x_tile - m_movingSelectionOffset.x * tileSize.x,
                    y_tile - m_movingSelectionOffset.y * tileSize.y
                });
            }
        }

        return true;
    }
    return false;
}

bool sections::MapSection::UpdateDragDrawing(PointerType pointerType, program::ProgramContext& programContext)
{
    auto mapDocument = programContext.GetManager<file::FileManager>()->GetActiveDocument();
    auto tilesetSection = programContext.GetSection<TilesetSection>();
    auto tileSize = m_mapView->GetTileSize();
    auto selectionSize = tilesetSection->GetCursorSizeInPixels().value_or(sgc::graphics::PixelSize2D{tileSize.x, tileSize.y});
    auto cursorTileSize = sgc::tile::TileSize2D{
        static_cast<sgc::math::ival>(selectionSize.x / tileSize.x),
        static_cast<sgc::math::ival>(selectionSize.y / tileSize.y)
    };

    auto cursorTilePositionOnTileset = m_mapView->PixelsToTiles(                
        tilesetSection->GetCursorPositionInPixels().value_or(sgc::graphics::PixelPosition2D{0, 0})
    );

    auto cursorPositionOnMap = m_mapView->GetCursorPositionInTiles();

    if(m_isPainting.IsLockedBy(pointerType)) {
        m_brush.PaintExecuteChange(
            *mapDocument,
            *m_mapView->GetTileset(),
            cursorPositionOnMap,
            cursorTilePositionOnTileset,
            cursorTileSize,
            this
        );

        return true;
    }
    return false;
}

bool sections::MapSection::UpdateOnCursorDown(PointerType pointerType, file::MapDocument *mapDocument, program::ProgramContext& programContext)
{
    auto cursorPositionOnMap = m_mapView->GetCursorPositionInTiles();
    auto tileset = m_mapView->GetTileset();
    auto tileSize = tileset->GetTileSize();

    if(m_brush.GetPaintMode() == editor_tools::PaintMode::Select && m_canMoveSelection && !m_isMovingSelection.IsLocked())
    {                
        auto selectionPos = m_mapView->GetSelectionPositionInTiles();
        auto selectionSize = m_mapView->GetSelectionSizeInTiles();

        if(cursorPositionOnMap.x >= selectionPos.x && cursorPositionOnMap.x <= selectionPos.x + selectionSize.x &&
            cursorPositionOnMap.y >= selectionPos.y && cursorPositionOnMap.y <= selectionPos.y + selectionSize.y)
        {                    
            m_isMovingSelection.Acquire(pointerType);

            m_movingSelectionOffset = {
                cursorPositionOnMap.x - selectionPos.x,
                cursorPositionOnMap.y - selectionPos.y
            };

            auto layerManager = mapDocument->GetLayerManager();
            auto layers = layerManager->GetLayers();
            auto selectionLayers = m_mapView->GetSelectionLayers();
            auto activeLayerIndex = layerManager->GetActiveLayerIndex();                                        

            auto iterateTiles = [&tileSize, &selectionPos, &selectionSize, &selectionLayers, this](program::LayerItem &layerItem, size_t renderingIndex, size_t storageIndex)
            {
                auto selectionStorage = std::make_shared<sgc::data::StaticTileStorage>(selectionSize);
                for(sgc::math::ival y = 0; y < selectionSize.y; ++y) {
                    for(sgc::math::ival x = 0; x < selectionSize.x; ++x) 
                    {
                        auto tilePos = sgc::tile::TilePosition2D{
                            selectionPos.x + x,
                            selectionPos.y + y
                        };

                        auto tileId = layerItem.storage->GetTileAt(tilePos);
                        selectionStorage->SetTileAt({x, y}, tileId);                                
                    }
                }

                m_selectionMovedStorage[storageIndex] = selectionStorage;
                
                auto selectionImage = std::make_shared<sgc::graphics::TiledImage>(
                    std::make_shared<sgc::graphics::TiledLayer>(
                        m_mapView->GetTileset(),
                        selectionStorage
                    )
                );

                selectionImage->SetPositionPixels(sgc::math::vec2{
                    selectionPos.x * tileSize.x,
                    selectionPos.y * tileSize.y
                });
                selectionImage->SetAlpha(layerItem.transparency);
                selectionLayers->Set(renderingIndex, selectionImage);
            };

            switch(GetSelectionMode())
            {
                case editor_tools::SelectionMode::SingleLayer:
                {                            
                    iterateTiles(layers[activeLayerIndex], layers.size() - activeLayerIndex - 1, activeLayerIndex);
                    break;
                }                        

                case editor_tools::SelectionMode::AllLayers:
                {
                    for(size_t i = 0; i < layers.size(); i++) {
                        iterateTiles(layers[i], layers.size() - i - 1, i);
                    }
                    break;
                }

                case editor_tools::SelectionMode::VisibleLayers:
                {
                    for(size_t i = 0; i < layers.size(); i++) {
                        if(layers[i].visible) {
                            iterateTiles(layers[i], layers.size() - i - 1, i);
                        }
                    }
                    break;
                }
            }

            programContext.GetManager<action::ActionManager>()->Find(action::ActionType::SelectionClear)->Execute(programContext);

            return true;
        }
    }

    if(!m_isPainting.IsLocked()) {
        m_isPainting.Acquire(pointerType);
    }

    if(m_brush.NeedsRedraw()) {
        return true;
    }
    
    return false;
}

bool sections::MapSection::UpdateOnCursorUp(PointerType pointerType, file::MapDocument *mapDocument)
{
    auto returnFlag = false;

    if(m_isMovingSelection.Release(pointerType)) 
    {
        auto selectionLayers = m_mapView->GetSelectionLayers();
        auto selectionPos = m_mapView->GetSelectionPositionInTiles();
        auto selectionSize = m_mapView->GetSelectionSizeInTiles();
        auto layerManager = mapDocument->GetLayerManager();
        auto layers = layerManager->GetLayers();                
        command::MultilayerTileChangesType tileChanges;

        for(auto &[storageIndex, storage] : m_selectionMovedStorage)
        {
            for(sgc::math::ival y = 0; y < selectionSize.y; ++y) {
                for(sgc::math::ival x = 0; x < selectionSize.x; ++x)
                {
                    auto tilePos = sgc::tile::TilePosition2D{
                        selectionPos.x + x,
                        selectionPos.y + y
                    };

                    auto movedTileId = storage->GetTileAt({x, y});
                    auto currentTileId = layers[storageIndex].storage->GetTileAt(tilePos);
                    
                    tileChanges[storageIndex][tilePos] = {
                        storageIndex,
                        tilePos,
                        currentTileId,
                        movedTileId.value_or(currentTileId.value_or(m_mapView->GetTileset()->TileIdCount()))
                    };
                }
            }
        }

        mapDocument->GetCommandManager()->Execute(std::make_unique<command::PaintSelectionCommand>(
            *mapDocument,
            tileChanges
        ));
        
        m_selectionMovedStorage.clear();
        selectionLayers->Clear();

        Update();
        returnFlag = true;
    }

    if(m_isPainting.Release(pointerType)) 
    {
        m_brush.PaintCommitChanges(*mapDocument);
        returnFlag = true;
    }

    return returnFlag;
}

void sections::MapSection::PointerUpdate(PointerType pointerType, sgc::graphics::PixelPosition2D pointerPosition, program::ProgramContext& programContext)
{    
    bool shouldUpdate = false;

    if(m_panningPointerType == pointerType)
    {
        shouldUpdate = PanningUpdate(pointerType, pointerPosition);
    }
    else if(m_paintingPointerType == pointerType)
    {
        auto result = UpdateCursorPosition(pointerPosition, programContext);
        shouldUpdate = result;
        result = UpdateSelectionMove(pointerType, pointerPosition);
        shouldUpdate = shouldUpdate || result;
        result = UpdateDragDrawing(pointerType, programContext);
        shouldUpdate = shouldUpdate || result;
    }
    else if(IsMouse(pointerType))
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

bool sections::PointerLock::IsLockedBy(PointerType pointerType) const {
    return m_isLocked && m_pointerType == pointerType;
}

bool sections::PointerLock::Acquire(PointerType pointerType) {
    if(m_isLocked) {
        return false;
    }

    m_pointerType = pointerType;
    m_isLocked = true;
    return true;
}

bool sections::PointerLock::Release(PointerType pointerType) {
    if(m_pointerType == pointerType) {
        m_isLocked = false;
        return true;
    }
    return false;
}

void sections::MapSection::PanningDown(PointerType pointerType, sgc::graphics::PixelPosition2D position)
{
    if(m_isPanning.IsLocked())
        return;

    m_isPanning.Acquire(pointerType);

    m_lastMousePosPan.x = position.x;
    m_lastMousePosPan.y = position.y;
}

void sections::MapSection::PanningUp(PointerType pointerType)
{    
    m_isPanning.Release(pointerType);
}

bool sections::MapSection::PanningUpdate(PointerType pointerType, sgc::graphics::PixelPosition2D position)
{
    if(!m_isPanning.IsLockedBy(pointerType))
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

void sections::MapSection::SetPaintingPointerType(PointerType pointerType)
{
    m_paintingPointerType = pointerType;
}

void sections::MapSection::SetPanningPointerType(PointerType pointerType)
{
    m_panningPointerType = pointerType;
}

sections::PointerType sections::MapSection::GetPaintingPointerType() const
{
    return m_paintingPointerType;
}

sections::PointerType sections::MapSection::GetPanningPointerType() const
{
    return m_panningPointerType;
}

void sections::MapSection::PointerDown(PointerType pointerType, file::MapDocument *mapDocument, sgc::graphics::PixelPosition2D position, program::ProgramContext& programContext)
{
    if(m_panningPointerType == pointerType) 
    {
        PanningDown(pointerType, position);
    }
    else if(m_paintingPointerType == pointerType) 
    {
        bool shouldUpdate = UpdateOnCursorDown(pointerType, mapDocument, programContext);
        PointerUpdate(pointerType, position, programContext);

        if(m_brush.NeedsRedraw() || shouldUpdate) {
            Update();
        }
    }
}

void sections::MapSection::PointerUp(PointerType pointerType, file::MapDocument *mapDocument, program::ProgramContext& programContext)
{
    if(m_isPanning.IsLockedBy(pointerType))
    {
        PanningUp(pointerType);
    }

    if(m_isPainting.IsLockedBy(pointerType))
    {
        UpdateOnCursorUp(pointerType, mapDocument);
    }
}
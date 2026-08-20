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

sections::MapSection::MapSection(program::ProgramContext& programContext) :
    Section{L"MapView", win32_program::ControlId::MapView, *programContext.mainWindowContext},
    m_mapView{std::make_unique<sgc_view::MapView>(GetHwnd())},
    m_brush{*programContext.selectionRectangleOnTileset}
{
    AttachView(*m_mapView);

    m_mapView->RegisterOnCursorPositionChangedCallback([this](sgc::math::vec2 position) 
    {
        auto &programContext = program::GetProgramContext();
        auto &statusSection = programContext.statusSection;
        auto mapDocument = programContext.fileManager->GetActiveDocument();
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
        auto &statusSection = programContext.statusSection;
        auto tileSize = m_mapView->GetTileSize();

        statusSection->SetStatusSelectionSize({
            size.x / tileSize.x,
            size.y / tileSize.y
        });
    });
}

void sections::MapSection::Update()
{    
    Redraw();
    if (m_mapView != nullptr) {        
        m_mapView->Render();
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
    auto mapDocument = programContext.fileManager->GetActiveDocument();
    auto &tilesetSection = programContext.tilesetSection;

    if(mapDocument == nullptr || !mapDocument->IsEditable())
    {
        return DefSubclassProc(hwnd, msg, wparam, lparam);
    }

    auto SetCaptureHelper = [this](HWND hwnd) {
        if (!m_isCaptured) {
            SetCapture(hwnd);
            m_isCaptured = true;
        }
    };

    auto ReleaseCaptureHelper = [this]() {
        if (m_isCaptured) {
            ReleaseCapture();
            m_isCaptured = false;
        }
    };

    auto IsMouseCaptured = [this]() -> bool {
        return m_isCaptured;
    };

    switch (msg)
    {
        case WM_LBUTTONDOWN:
        {
            if(IsMouseCaptured()) {
                return 0;
            }

            auto cursorPositionOnMap = m_mapView->GetCursorPositionInTiles();
            auto tileset = m_mapView->GetTileset();
            auto tileSize = tileset->GetTileSize();

            if(m_brush.GetPaintMode() == editor_tools::PaintMode::Select && m_canMoveSelection)
            {                
                auto selectionPos = m_mapView->GetSelectionPositionInTiles();
                auto selectionSize = m_mapView->GetSelectionSizeInTiles();

                if(cursorPositionOnMap.x >= selectionPos.x && cursorPositionOnMap.x <= selectionPos.x + selectionSize.x &&
                   cursorPositionOnMap.y >= selectionPos.y && cursorPositionOnMap.y <= selectionPos.y + selectionSize.y)
                {                    
                    m_isMovingSelection = true;

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

                    programContext.actionManager->Find(action::ActionType::SelectionClear)->Execute(programContext);

                    return 0;
                }
            }            
            
            auto mousePos = m_mapView->PixelsToTiles(
            sgc::math::vec2{ 
                GET_X_LPARAM(lparam),
                GET_Y_LPARAM(lparam)
            });
            
            auto currentTilePosition = tilesetSection->GetCursorPositionInPixels().value_or(sgc::graphics::PixelPosition2D{0, 0});
            auto tileWidth = static_cast<sgc::math::ival>(tilesetSection->GetCursorSizeInPixels().value_or(sgc::graphics::PixelSize2D{tileSize.x, tileSize.y}).x / tileSize.x);
            auto tileHeight = static_cast<sgc::math::ival>(tilesetSection->GetCursorSizeInPixels().value_or(sgc::graphics::PixelSize2D{tileSize.x, tileSize.y}).y / tileSize.y);

            m_brush.PaintExecuteChange(
                *mapDocument,
                *tileset,
                cursorPositionOnMap,
                sgc::tile::TilePosition2D{
                    static_cast<sgc::math::ival>(currentTilePosition.x / tileSize.x),
                    static_cast<sgc::math::ival>(currentTilePosition.y / tileSize.y)
                },
                sgc::tile::TileSize2D{tileWidth, tileHeight},
                this
            );
            
            m_isPainting = true;
            SetCaptureHelper(hwnd);    

            if(m_brush.NeedsRedraw()) {
                Update();
            }       
            
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            if(m_isPanning) {
                auto x = static_cast<sgc::math::ival>(GET_X_LPARAM(lparam));
                auto y = static_cast<sgc::math::ival>(GET_Y_LPARAM(lparam));

                auto deltaX = m_mapView->ScaleForZoom(static_cast<float>(x - m_lastMousePosPan.x));
                auto deltaY = m_mapView->ScaleForZoom(static_cast<float>(y - m_lastMousePosPan.y));

                m_mapView->ChangeCameraPositionSingles(
                    deltaX,
                    deltaY
                );

                m_lastMousePosPan.x = x;
                m_lastMousePosPan.y = y;

                Update();
                return 0;
            }

            bool shouldUpdate = false;

            auto cameraPosition = m_mapView->GetCameraPositionSingles();

            auto screenPosition = sgc::math::fvec2{
                static_cast<float>(GET_X_LPARAM(lparam)),
                static_cast<float>(GET_Y_LPARAM(lparam))
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
                
                auto selection = tilesetSection->GetCursorSizeInPixels().value_or(sgc::graphics::PixelSize2D{tileSize.x, tileSize.y});

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

                    /* case editor_tools::PaintMode::Select:
                        // select sets the cursor size on left button up
                        break; */
                }                

                m_mapView->SetCursorPositionInPixels({
                    x_tile,
                    y_tile
                });

                if(m_isMovingSelection) 
                {
                    auto selectionLayers = m_mapView->GetSelectionLayers();

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
                }
                
                shouldUpdate = true;                
            }

            auto selectionSize = tilesetSection->GetCursorSizeInPixels().value_or(sgc::graphics::PixelSize2D{tileSize.x, tileSize.y});
            auto cursorTileSize = sgc::tile::TileSize2D{
                static_cast<sgc::math::ival>(selectionSize.x / tileSize.x),
                static_cast<sgc::math::ival>(selectionSize.y / tileSize.y)
            };

            auto cursorTilePositionOnTileset = m_mapView->PixelsToTiles(                
                tilesetSection->GetCursorPositionInPixels().value_or(sgc::graphics::PixelPosition2D{0, 0})
            );

            auto cursorPositionOnMap = m_mapView->GetCursorPositionInTiles();

            if(m_isPainting) {
                m_brush.PaintExecuteChange(
                    *mapDocument,
                    *m_mapView->GetTileset(),
                    cursorPositionOnMap,
                    cursorTilePositionOnTileset,
                    cursorTileSize,
                    this
                );
            }

            if(m_brush.NeedsRedraw() || shouldUpdate) {
                Update();
            }

            return 0;
        }

        case WM_LBUTTONUP:
        {
            if(m_isMovingSelection) 
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

                m_isMovingSelection = false;
                m_selectionMovedStorage.clear();
                selectionLayers->Clear();

                Update();
            }
            
            if(m_isPainting) 
            {
                m_isPainting = false;

                m_brush.PaintCommitChanges(*mapDocument);

                ReleaseCaptureHelper();
            }

            return 0;
        }

        case WM_MBUTTONUP:
        {
            if (m_isPanning)
            {
                m_isPanning = false;
                ReleaseCaptureHelper();
            }

            return 0;
        }

        case WM_MBUTTONDOWN:
        {
            if(IsMouseCaptured()) {
                break;
            }

            m_isPanning = true;

            m_lastMousePosPan.x = static_cast<sgc::math::ival>(GET_X_LPARAM(lparam));
            m_lastMousePosPan.y = static_cast<sgc::math::ival>(GET_Y_LPARAM(lparam));

            SetCaptureHelper(hwnd);

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
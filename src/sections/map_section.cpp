#include "sections/map_section.hpp"
#include "program/program.hpp"
#include "command/paint_tiles_command.hpp"
#include "command/command_manager.hpp"

#include <sgc/data/resourcemanager.hpp>
#include <sgc/asset/assetloader.hpp>
#include <sgc/asset/chunkedtilestorageserializer.hpp>
#include <sgc/asset/chunkedtilestorageassetbuilder.hpp>
#include <sgc/coordinates/screenworld.hpp>
#include <sgc/sdl/sdl_win32.hpp>
#include <fstream>
#include <filesystem>
#include <windowsx.h>
#include <commctrl.h>
#include <cmath>

sections::MapSection::MapSection(program::ProgramContext& programContext) :
    Section{L"MapView", win32_program::ControlId::MapView, *programContext.mainWindowContext},
    m_mapView{std::make_unique<sgc_view::MapView>(GetHwnd())}
{
    AttachView(*m_mapView);
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

void sections::MapSection::LoadMap(const std::filesystem::path& path)
{
    if (m_mapView == nullptr) {
        return;
    }

    std::ifstream file(path, std::ios::binary);
    std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    sgc::data::ResourceContext rc;
    sgc::data::ResourceManager rm;

    auto storage = sgc::asset::AssetLoader<sgc::data::ChunkedTileStorage>::Load(
        bytes,
        rc,
        rm
    );
}

void sections::MapSection::SaveMap(const std::filesystem::path& path)
{
    if (m_mapView == nullptr) {
        return;
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
    auto mapDocument = programContext.fileManager->GetSelectedDocument();
    auto &tilesetSection = programContext.tilesetSection;

    if(mapDocument == nullptr)
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

            auto mapDocument = programContext.fileManager->GetSelectedDocument();

            if(mapDocument == nullptr) {
                return 0;
            }

            auto tileset = m_mapView->GetTileset();
            auto tileSize = tileset->GetTileSize();

            auto mousePos = m_mapView->PixelsToTiles(
            sgc::math::vec2{ 
                GET_X_LPARAM(lparam),
                GET_Y_LPARAM(lparam)
            });

            // auto currentTilePosition = programContext.selectionRectangleOnTileset->GetPosition();          

            // auto tileWidth = static_cast<sgc::math::ival>(programContext.selectionRectangleOnTileset->GetSize().x / tileSize.x);
            // auto tileHeight = static_cast<sgc::math::ival>(programContext.selectionRectangleOnTileset->GetSize().y / tileSize.y);
            
            auto currentTilePosition = tilesetSection->GetCursorPositionInPixels().value_or(sgc::graphics::PixelPosition2D{0, 0});
            auto tileWidth = static_cast<sgc::math::ival>(tilesetSection->GetCursorSizeInPixels().value_or(sgc::graphics::PixelSize2D{tileSize.x, tileSize.y}).x / tileSize.x);
            auto tileHeight = static_cast<sgc::math::ival>(tilesetSection->GetCursorSizeInPixels().value_or(sgc::graphics::PixelSize2D{tileSize.x, tileSize.y}).y / tileSize.y);

            auto layerManager = mapDocument->GetLayerManager();
            auto currentLayer = mapDocument->GetCurrentLayerStorage();

            if(currentLayer == nullptr) {
                return 0;
            }

            auto cursorPositionOnMap = m_mapView->GetCursorPositionInTiles();

            m_selectionTileStart = cursorPositionOnMap;
            
            std::vector<command::TileChange> tileChanges;
            
            if(m_paintStrokeCommand != nullptr) {
                m_paintStrokeCommand.reset();
            }

            m_paintStrokeCommand = std::make_unique<command::PaintStrokeCommand>(layerManager->GetActiveLayerIndex());

            for(sgc::math::ival _x = 0; _x < tileWidth; ++_x) {
                for(sgc::math::ival _y = 0; _y < tileHeight; ++_y) {
                    auto tileX = cursorPositionOnMap.x + _x;
                    auto tileY = cursorPositionOnMap.y + _y;
                    auto currentTileId = tileset->ToTileId(static_cast<sgc::math::uval>(currentTilePosition.x / tileSize.x) + _x, static_cast<sgc::math::uval>(currentTilePosition.y / tileSize.y) + _y);

                    if( !m_checkTileBeforePainting || currentLayer->GetTileAt({ tileX, tileY }).has_value()) {

                        command::TileChange change{
                            { tileX, tileY },
                            currentLayer->GetTileAt({ tileX, tileY }),
                            currentTileId
                        };                        
                        m_paintStrokeCommand->ExecuteTileChange(change);

                    }                    
                }
            }

            m_isPainting = true;
            SetCaptureHelper(hwnd);        

            Update();            
            
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            if(m_isPanning) {
                auto x = static_cast<sgc::math::ival>(GET_X_LPARAM(lparam));
                auto y = static_cast<sgc::math::ival>(GET_Y_LPARAM(lparam));

                auto deltaX = x - m_lastMousePosPan.x;
                auto deltaY = y - m_lastMousePosPan.y;

                m_mapView->ChangeCameraPositionSingles(
                    static_cast<float>(deltaX),
                    static_cast<float>(deltaY)
                );

                m_lastMousePosPan.x = x;
                m_lastMousePosPan.y = y;

                Update();
                return 0;
            }

            auto mapDocument = programContext.fileManager->GetSelectedDocument();

            if(mapDocument == nullptr) {
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
                
                // auto selection = programContext.selectionRectangleOnTileset->GetSize();
                auto selection = tilesetSection->GetCursorSizeInPixels().value_or(sgc::graphics::PixelSize2D{tileSize.x, tileSize.y});

                m_mapView->SetCursorSizeInPixels({
                    selection.x,
                    selection.y
                });

                m_mapView->SetCursorPositionInPixels({
                    x_tile,
                    y_tile
                });
                
                shouldUpdate = true;                
            }

            if (!m_isPainting){
                if(shouldUpdate) {
                    Update();
                }
                break;
            }                

            auto cursorTileSize = m_mapView->GetCursorSizeInTiles();

            auto cursorTilePositionOnTileset = m_mapView->PixelsToTiles(
                // programContext.selectionRectangleOnTileset->GetPosition()
                tilesetSection->GetCursorPositionInPixels().value_or(sgc::graphics::PixelPosition2D{0, 0})
            );
            
            auto currentLayer = mapDocument->GetCurrentLayerStorage();

            if(currentLayer == nullptr) {
                return 0;
            }

            auto cursorPositionOnMap = m_mapView->GetCursorPositionInTiles();

            auto absmod = [](sgc::math::ival value, sgc::math::ival mod) -> sgc::math::ival {
                return ((value % mod) + mod) % mod;
            };

            for (sgc::math::ival x = 0; x < static_cast<sgc::math::ival>(cursorTileSize.x); ++x){
                for (sgc::math::ival y = 0; y < static_cast<sgc::math::ival>(cursorTileSize.y); ++y){

                    auto tileMapX = cursorPositionOnMap.x + x;
                    auto tileMapY = cursorPositionOnMap.y + y;

                    auto deltaX = absmod(
                        tileMapX - m_selectionTileStart.x,
                        cursorTileSize.x
                    );

                    auto deltaY = absmod(
                        tileMapY - m_selectionTileStart.y,
                        cursorTileSize.y
                    );

                    auto tileId = m_mapView->GetTileset()->ToTileId(
                        cursorTilePositionOnTileset.x + deltaX,
                        cursorTilePositionOnTileset.y + deltaY
                    );

                   if( !m_checkTileBeforePainting || currentLayer->GetTileAt({ tileMapX, tileMapY }).has_value()) {

                        command::TileChange change{
                            { tileMapX, tileMapY },
                            currentLayer->GetTileAt({ tileMapX, tileMapY }),
                            tileId
                        };
                        
                        m_paintStrokeCommand->ExecuteTileChange(change);

                    }  
                }
            }

            Update();

            return 0;
        }

        case WM_LBUTTONUP:
        {
            if(m_isPainting) {
                m_isPainting = false;

                if(m_paintStrokeCommand != nullptr) {
                    programContext.commandManager->Commit(
                        std::move(m_paintStrokeCommand)
                    );
                }

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
    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

void sections::MapSection::SetCheckTileBeforePainting(bool check)
{
    m_checkTileBeforePainting = check;
}
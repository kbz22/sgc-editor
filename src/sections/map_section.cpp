#include "sections/map_section.hpp"
#include "program/program.hpp"
#include "command/paint_tiles_command.hpp"
#include "command/command_manager.hpp"

#include <sgc/data/resourcemanager.hpp>
#include <sgc/asset/assetloader.hpp>
#include <sgc/asset/chunkedtilestorageserializer.hpp>
#include <sgc/asset/chunkedtilestorageassetbuilder.hpp>
#include <sgc/coordinates/screenworld.hpp>
#include <fstream>
#include <filesystem>
#include <windowsx.h>
#include <commctrl.h>
#include <cmath>

sections::MapSection::MapSection(program::ProgramContext& programContext) :
    Section{L"MapView", win32_program::ControlId::MapView, *programContext.mainWindowContext},
    m_mapView{nullptr}
{}

void sections::MapSection::Update()
{    
    Redraw();
    if (m_mapView != nullptr) {        
        m_mapView->Render();
    }    
}

void sections::MapSection::Refresh(program::LayerManager& layerManager)
{    
    if (m_mapView != nullptr) {
        m_mapView->Refresh(layerManager);
    }
}

void sections::MapSection::LoadTileset(const std::filesystem::path& path, int tileWidth, int tileHeight)
{   
    if(m_mapView != nullptr) {
        m_mapView.reset();
    }
    
    m_mapView = std::make_unique<sgc_view::MapView>(GetHwnd(), path, tileWidth, tileHeight);    
    AttachView(*m_mapView);
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

    /* m_mapView->SetStorage(
        std::make_shared<sgc::data::ChunkedTileStorage>(std::move(*storage))
    ); */
}

void sections::MapSection::SaveMap(const std::filesystem::path& path)
{
    if (m_mapView == nullptr) {
        return;
    }    

    /* auto asset = sgc::asset::AssetBuilder<sgc::asset::ChunkedTileStorageAsset>::Build(        
        *(m_mapView->m_tileStorage)
    );

    auto bytes = sgc::asset::AssetSerializer<sgc::asset::ChunkedTileStorageAsset>::Serialize(
        asset
    );

    std::ofstream file(path, std::ios::binary);
    file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size()); */

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

    switch (msg)
    {
        case WM_LBUTTONDOWN:
        {
            auto tileset = m_mapView->GetTileset();
            auto tileSize = tileset->GetTileSize();

            auto mousePos = m_mapView->PixelsToTiles(
            sgc::math::vec2{ 
                GET_X_LPARAM(lparam),
                GET_Y_LPARAM(lparam)
            });

            auto currentTilePosition = programContext.selectionRectangleOnTileset->GetPosition();          

            auto tileWidth = static_cast<sgc::math::ival>(programContext.selectionRectangleOnTileset->GetSize().x / tileSize.x);
            auto tileHeight = static_cast<sgc::math::ival>(programContext.selectionRectangleOnTileset->GetSize().y / tileSize.y);            

            auto currentLayer = programContext.layerManager->GetLayers()[programContext.layerManager->GetActiveLayerIndex()];

            if(currentLayer.storage == nullptr) { //! check this out
                return 0;
            }

            auto cursorPositionOnMap = m_mapView->GetCursorPositionInTiles();

            m_selectionTileStart = cursorPositionOnMap;
            
            std::vector<command::TileChange> tileChanges;
            
            if(m_paintStrokeCommand != nullptr) {
                m_paintStrokeCommand.reset();
            }

            m_paintStrokeCommand = std::make_unique<command::PaintStrokeCommand>(programContext.layerManager->GetActiveLayerIndex());

            for(sgc::math::ival _x = 0; _x < tileWidth; ++_x) {
                for(sgc::math::ival _y = 0; _y < tileHeight; ++_y) {
                    auto tileX = cursorPositionOnMap.x + _x;
                    auto tileY = cursorPositionOnMap.y + _y;
                    auto currentTileId = tileset->ToTileId(static_cast<sgc::math::uval>(currentTilePosition.x / tileSize.x) + _x, static_cast<sgc::math::uval>(currentTilePosition.y / tileSize.y) + _y);

                    if( !m_checkTileBeforePainting || currentLayer.storage->GetTileAt({ tileX, tileY }).has_value()) {

                        command::TileChange change{
                            { tileX, tileY },
                            currentLayer.storage->GetTileAt({ tileX, tileY }),
                            currentTileId
                        };                        
                        m_paintStrokeCommand->ExecuteTileChange(change);

                    }                    
                }
            }

            m_isPainting = true;

            /* programContext.commandManager->Execute(
                std::make_unique<command::PaintTilesCommand>(
                    tileChanges,
                    *currentLayer.storage
                )
            ); */

            Update();            
            
            return 0;
        }

        case WM_MOUSEMOVE:
        {

            if(m_isPanning) {
                auto x = static_cast<sgc::math::ival>(GET_X_LPARAM(lparam));
                auto y = static_cast<sgc::math::ival>(GET_Y_LPARAM(lparam));

                auto deltaX = x - m_lastMousePos.x;
                auto deltaY = y - m_lastMousePos.y;

                m_mapView->ChangeCameraPositionSingles(
                    static_cast<float>(deltaX),
                    static_cast<float>(deltaY)
                );

                m_lastMousePos.x = x;
                m_lastMousePos.y = y;

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
                
                auto selection = programContext.selectionRectangleOnTileset->GetSize();

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
                programContext.selectionRectangleOnTileset->GetPosition()
            );

            auto currentLayer = programContext.layerManager->
                GetLayers()[programContext.layerManager->GetActiveLayerIndex()];

            if (currentLayer.storage == nullptr) {
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

                   if( !m_checkTileBeforePainting || currentLayer.storage->GetTileAt({ tileMapX, tileMapY }).has_value()) {

                        command::TileChange change{
                            { tileMapX, tileMapY },
                            currentLayer.storage->GetTileAt({ tileMapX, tileMapY }),
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
            m_isPainting = false;

            if(m_paintStrokeCommand != nullptr) {
                programContext.commandManager->Commit(
                    std::move(m_paintStrokeCommand)
                );
                // m_paintStrokeCommand.reset();
            }

            return 0;
        }

        case WM_MBUTTONUP:
        {
            if (m_isPanning)
            {
                m_isPanning = false;
                ReleaseCapture();
            }

            return 0;
        }

        case WM_MBUTTONDOWN:
        {
            m_isPanning = true;

            m_lastMousePos.x = static_cast<sgc::math::ival>(GET_X_LPARAM(lparam));
            m_lastMousePos.y = static_cast<sgc::math::ival>(GET_Y_LPARAM(lparam));

            SetCapture(hwnd);

            return 0;
        }
    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

void sections::MapSection::SetCheckTileBeforePainting(bool check)
{
    m_checkTileBeforePainting = check;
}
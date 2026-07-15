#include "sections/map_section.hpp"
#include "program/program.hpp"
#include <sgc/data/resourcemanager.hpp>
#include <sgc/asset/assetloader.hpp>
#include <sgc/asset/chunkedtilestorageserializer.hpp>
#include <sgc/asset/chunkedtilestorageassetbuilder.hpp>
#include <fstream>
#include <filesystem>
#include <windowsx.h>
#include <commctrl.h>

sections::MapSection::MapSection(win32_program::MainWindowContext& context) :
    Section{L"MapView", win32_program::ControlId::MapView, context},
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
                sgc::math::uvec2{ 
                    static_cast<sgc::math::uval>(GET_X_LPARAM(lparam)),
                    static_cast<sgc::math::uval>(GET_Y_LPARAM(lparam))
                });

            m_selectionTileStart = mousePos;

            auto currentTilePosition = programContext.selectionRectangleOnTileset->GetPosition();          

            auto tileWidth = static_cast<sgc::math::ival>(programContext.selectionRectangleOnTileset->GetSize().x / tileSize.x);
            auto tileHeight = static_cast<sgc::math::ival>(programContext.selectionRectangleOnTileset->GetSize().y / tileSize.y);            

            auto currentLayer = programContext.layerManager->GetLayers()[programContext.layerManager->GetActiveLayerIndex()];

            if(currentLayer.storage == nullptr) { //! check this out
                return 0;
            }

            for(sgc::math::ival _x = 0; _x < tileWidth; ++_x) {
                for(sgc::math::ival _y = 0; _y < tileHeight; ++_y) {
                    auto tileX = static_cast<sgc::math::ival>(m_selectionTileStart.x + _x);
                    auto tileY = static_cast<sgc::math::ival>(m_selectionTileStart.y + _y);
                    auto currentTileId = tileset->ToTileId(static_cast<sgc::math::uval>(currentTilePosition.x / tileSize.x) + _x, static_cast<sgc::math::uval>(currentTilePosition.y / tileSize.y) + _y);                    

                    if(currentLayer.storage->GetTileAt({ tileX, tileY }).has_value()) {
                        currentLayer.storage->SetTileAt({ tileX, tileY }, currentTileId);
                    }
                }
            }

            m_isPainting = true;            

            Update();
            
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            bool shouldUpdate = false;

            auto x = GET_X_LPARAM(lparam);
            auto y = GET_Y_LPARAM(lparam);
            auto tileSize = m_mapView->GetTileSize();

            auto x_tile = static_cast<sgc::math::ival>(x - (x % tileSize.x));
            auto y_tile = static_cast<sgc::math::ival>(y - (y % tileSize.y));

            auto position = m_mapView->GetCursorPositionInTiles();
            if (position.x != x_tile || position.y != y_tile) {
                
                auto selection = programContext.selectionRectangleOnTileset->GetSize();

                m_mapView->SetCursorSizeInPixels({
                    static_cast<sgc::math::uval>(selection.x),
                    static_cast<sgc::math::uval>(selection.y)
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
            auto cursorTilePositionOnTileset = m_mapView->PixelsToTiles(programContext.selectionRectangleOnTileset->GetPosition());
            auto mousePositionInTiles = m_mapView->PixelsToTiles(sgc::math::uvec2{ static_cast<sgc::math::uval>(x), static_cast<sgc::math::uval>(y) });

            auto currentLayer = programContext.layerManager->GetLayers()[programContext.layerManager->GetActiveLayerIndex()];

            if(currentLayer.storage == nullptr) { //! check this out
                return 0;
            }

            for (sgc::math::ival _x = 0; _x < static_cast<sgc::math::ival>(cursorTileSize.x); ++_x){
                for (sgc::math::ival _y = 0; _y < static_cast<sgc::math::ival>(cursorTileSize.y); ++_y){

                    auto absmod = [](sgc::math::ival value, sgc::math::ival mod) -> sgc::math::ival {
                        return ((value % mod) + mod) % mod;
                    };

                    auto tileMapX = static_cast<sgc::math::ival>(mousePositionInTiles.x) + _x;
                    auto tileMapY = static_cast<sgc::math::ival>(mousePositionInTiles.y) + _y;

                    auto deltaX = absmod(
                        tileMapX - static_cast<sgc::math::ival>(m_selectionTileStart.x),
                        static_cast<sgc::math::ival>(cursorTileSize.x)
                    );
                    auto deltaY = absmod(
                        tileMapY - static_cast<sgc::math::ival>(m_selectionTileStart.y),
                        static_cast<sgc::math::ival>(cursorTileSize.y)
                    );                    

                    auto tileId = m_mapView->GetTileset()->ToTileId(
                        cursorTilePositionOnTileset.x + deltaX,
                        cursorTilePositionOnTileset.y + deltaY
                    );

                    if(currentLayer.storage->GetTileAt({ tileMapX, tileMapY }).has_value()) {
                        currentLayer.storage->SetTileAt(
                            { tileMapX, tileMapY },
                            tileId
                        );
                    }
                }
            }

            Update();

            return 0;
        }

        case WM_LBUTTONUP:
        {
            m_isPainting = false;
            return 0;
        }
    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}
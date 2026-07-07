#pragma once

#include "win32_section/section.hpp"
#include "sgc_view/map_view.hpp"

namespace win32_section {

    class MapSection : public Section
    {
        private:
            std::unique_ptr<sgc_view::MapView> m_mapView = nullptr;

        public:
            MapSection(win32_program::Win32Context& context);
            
            void Update() override;
            void HandleSectionResize() override;

            void LoadTileset(const std::filesystem::path& path, int tileWidth = defaults::tileSize, int tileHeight = defaults::tileSize);
            void LoadMap(const std::filesystem::path& path);
            void SaveMap(const std::filesystem::path& path);

            void SetTile(sgc::tile::TilesetPosition2D tilePos, sgc::tile::TileId tileId);
    };

}
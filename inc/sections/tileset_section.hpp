#pragma once

#include <filesystem>
#include <memory>

#include "sections/section.hpp"
#include "sgc_view/tileset_view.hpp"
#include "defaults.hpp"

namespace sections {

    class TilesetSection : public Section
    {
        private:
            std::unique_ptr<sgc_view::TilesetView> m_tilesetView = nullptr;
        
        public:
            TilesetSection(win32_program::MainWindowContext& context);
            
            void Update() override;
            void HandleSectionResize() override;

            void LoadTileset(const std::filesystem::path& path, int tileWidth = defaults::tileSize, int tileHeight = defaults::tileSize);
            void ClearTileset();
    };

}
#pragma once

#include <filesystem>
#include <memory>

#include "win32_section/section.hpp"
#include "sgc_view/tileset_view.hpp"
#include "defaults.hpp"

namespace win32_section {

    class TilesetSection : public Section
    {
        private:
            std::unique_ptr<sgc_view::TilesetView> m_tilesetView = nullptr;
        
        public:
            TilesetSection(win32_program::Win32Context& context);
            
            void Update() override;
            void HandleSectionResize() override;

            void LoadTileset(const std::filesystem::path& path, int tileWidth = defaults::tileSize, int tileHeight = defaults::tileSize);
            void ClearTileset();
    };

}
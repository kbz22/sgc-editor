#pragma once

#include <filesystem>
#include <memory>
#include <windows.h>
#include <sgc/math/vector.hpp>

#include "sections/section.hpp"
#include "sgc_view/tileset_view.hpp"
#include "defaults.hpp"

namespace program {
    struct ProgramContext;
}

namespace sections {

    class TilesetSection : public Section
    {
        private:
            std::unique_ptr<sgc_view::TilesetView> m_tilesetView = nullptr;

            LRESULT HandleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
            bool m_selectionActive = false;        
            sgc::math::vec2 m_selectionTileStart = { 0, 0 };    
            sgc::math::vec2 m_selectionTileSize = { 0, 0 };
        
        public:
            TilesetSection(program::ProgramContext& programContext);
            
            void Update() override;
            void HandleSectionResize() override;

            void LoadTileset(const std::filesystem::path& path, int tileWidth, int tileHeight);
            void ClearTileset();
    };

}
#pragma once

#include <sgc_view/sgc_view.hpp>
#include <sgc/graphics/rectangle.hpp>
#include "defaults.hpp"

namespace sgc_view 
{
    class TilesetView : public SgcView
    {
        private:
            math::u64 m_gridWidth = 0;
            math::u64 m_gridHeight = 0;
            graphics::Rectangle m_highlightedTile = graphics::Rectangle{ 0, 0, defaults::tileSize, defaults::tileSize };

        protected:
            LRESULT HandleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;

        public:
            TilesetView(HWND hwnd);
            ~TilesetView();

            bool LoadTileset(const std::wstring& path, int tileWidth = defaults::tileSize, int tileHeight = defaults::tileSize) override;
            void Render() override;
    };    
}
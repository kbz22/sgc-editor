#pragma once

#include <sgc_view/sgc_view.hpp>
#include <sgc/graphics/rectangle.hpp>
#include <sgc/graphics/tiledlayer.hpp>
#include "defaults.hpp"

namespace sgc_view 
{
    class TilesetView : public SgcView
    {
        private:
            math::u64 m_gridWidth = 0;
            math::u64 m_gridHeight = 0;
            std::shared_ptr<graphics::TiledLayer> m_layer = nullptr;
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
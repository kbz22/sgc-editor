#pragma once

#include <sgc_view/sgc_view.hpp>
#include <sgc/image/rectangle.hpp>
#include "defaults.hpp"

namespace sgc_view 
{
    class TilesetView : public SgcView
    {
        private:
            types::unsignedint_t m_gridWidth = 0;
            types::unsignedint_t m_gridHeight = 0;
            image::Rectangle m_highlightedTile = image::Rectangle{ 0, 0, defaults::tileSize, defaults::tileSize };

        protected:
            LRESULT HandleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;

        public:
            TilesetView(HWND hwnd);
            ~TilesetView();

            bool LoadTileset(const std::wstring& path, int tileWidth = defaults::tileSize, int tileHeight = defaults::tileSize) override;
            void Render() override;
    };    
}
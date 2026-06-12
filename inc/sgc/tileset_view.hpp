#pragma once

#include <sgc/sgc_view.hpp>

namespace sgc 
{
    class TilesetView : public SgcView
    {
        private:
            types::unsignedint_t m_gridWidth = 0;
            types::unsignedint_t m_gridHeight = 0;

        public:
            TilesetView(HWND hwnd);
            ~TilesetView();

            bool LoadTileset(const std::wstring& path, int tileWidth = defaults::tileSize, int tileHeight = defaults::tileSize) override;
    };    
}
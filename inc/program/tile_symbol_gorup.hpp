#pragma once

#include "program/sgc_tileset.hpp"
#include <sgc/tile/tile.hpp>
#include <vector>

namespace program
{
    class TileSymbolGroup
    {
        private:
            std::vector<sgc::tile::TilePosition2D> m_tileSymbols{};
            sgc::tile::TileSize2D m_symbolSize = {0,0};
            size_t m_currentSymbolIndex;

        public:
            TileSymbolGroup();

            void SetSize(sgc::tile::TileSize2D symbolSize);
            void AddSymbol(sgc::tile::TilePosition2D positionOnTileset);
            void ClearSymbols();

            sgc::tile::TileSize2D GetSize() const;
            std::vector<sgc::tile::TilePosition2D> GetPositions() const;

            std::vector<sgc::tile::TileId> PeekSymbol(const SgcTileset &sgcTileset) const;
            std::vector<sgc::tile::TileId> NextSymbol(const SgcTileset &sgcTileset);

    };
}
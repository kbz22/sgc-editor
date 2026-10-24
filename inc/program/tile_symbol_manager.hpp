#pragma once

#include "program/tile_symbol_gorup.hpp"
#include "program/sgc_tileset.hpp"
#include <map>
#include <string>

namespace program
{
    struct TileSymbolEntry
    {
        std::wstring groupName;        
        TileSymbolGroup group;
    };

    class TileSymbolManager
    {
        private:
            std::vector<TileSymbolEntry> m_tileSymbolGroups;
            size_t m_currentGroupIndex;

        public:
            void CreateSymbolGroup(std::wstring groupName, SgcTileset &sgcTileset, const sgc::tile::TileSize2D &symbolSize, const std::vector<sgc::tile::TilePosition2D> &positions);

            TileSymbolGroup* PeekSymbolGroup();
            TileSymbolGroup* NextSymbolGroup();
    };
}
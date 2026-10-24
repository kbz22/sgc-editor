#include "program/tile_symbol_gorup.hpp"

program::TileSymbolGroup::TileSymbolGroup(SgcTileset &sgcTileset) :
    m_sgcTileset{sgcTileset}
{}

void program::TileSymbolGroup::SetSize(sgc::tile::TileSize2D symbolSize)
{
    m_symbolSize = symbolSize;
}

void program::TileSymbolGroup::AddSymbol(sgc::tile::TilePosition2D positionOnTileset)
{
    m_tileSymbols.push_back(positionOnTileset);
}

void program::TileSymbolGroup::ClearSymbols()
{
    m_tileSymbols.clear();
}

sgc::tile::TileSize2D program::TileSymbolGroup::GetSize() const
{
    return m_symbolSize;
}

std::vector<sgc::tile::TileId> program::TileSymbolGroup::PeekSymbol() const
{
    if(m_currentSymbolIndex >= m_tileSymbols.size())
        return {};

    auto &tilePosition = m_tileSymbols[m_currentSymbolIndex];
    std::vector<sgc::tile::TileId> tiles{};

    for(int x=0; x < m_symbolSize.x ; x++){
        for(int y=0; y < m_symbolSize.y ; y++)
        {
            sgc::tile::TilePosition2D positionOnSymbol{x,y};
            auto checkTile = m_sgcTileset.GetTileId(tilePosition + positionOnSymbol);
            if(checkTile.has_value())
                tiles.push_back(checkTile.value());
        }   
    }
    
    return tiles;
}

std::vector<sgc::tile::TileId> program::TileSymbolGroup::NextSymbol()
{
    auto returnValue = PeekSymbol();
    m_currentSymbolIndex = (m_currentSymbolIndex + 1) % m_tileSymbols.size();
    return returnValue;
}

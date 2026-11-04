#include "program/tile_symbol_gorup.hpp"

program::TileSymbolGroup::TileSymbolGroup()
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

std::vector<sgc::tile::TilePosition2D> program::TileSymbolGroup::GetPositions() const
{
    return m_tileSymbols;
}

/* std::vector<sgc::tile::TileId> program::TileSymbolGroup::PeekSymbol(const SgcTileset &sgcTileset) const
{
    if(m_currentSymbolIndex >= m_tileSymbols.size())
        return {};

    auto &tilePosition = m_tileSymbols[m_currentSymbolIndex];
    std::vector<sgc::tile::TileId> tiles{};

    for(int x=0; x < m_symbolSize.x ; x++){
        for(int y=0; y < m_symbolSize.y ; y++)
        {
            sgc::tile::TilePosition2D positionOnSymbol{x,y};
            auto checkTile = sgcTileset.GetTileId(tilePosition + positionOnSymbol);
            if(checkTile.has_value())
                tiles.push_back(checkTile.value());
        }   
    }
    
    return tiles;
}

std::vector<sgc::tile::TileId> program::TileSymbolGroup::NextSymbol(const SgcTileset &sgcTileset)
{
    auto returnValue = PeekSymbol(sgcTileset);
    m_currentSymbolIndex = (m_currentSymbolIndex + 1) % m_tileSymbols.size();
    return returnValue;
} */

std::optional<sgc::tile::TilePosition2D> program::TileSymbolGroup::PeekSymbol(const SgcTileset &sgcTileset) const
{
    if(m_currentSymbolIndex >= m_tileSymbols.size())
        return std::nullopt;

    return m_tileSymbols[m_currentSymbolIndex];
}

std::optional<sgc::tile::TilePosition2D> program::TileSymbolGroup::NextSymbol(const SgcTileset &sgcTileset)
{
    auto returnValue = PeekSymbol(sgcTileset);
    m_currentSymbolIndex = (m_currentSymbolIndex + 1) % m_tileSymbols.size();
    return returnValue;
}

#include "program/tile_symbol_gorup.hpp"

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

std::optional<sgc::tile::TilePosition2D> program::TileSymbolGroup::PeekSymbol(const SgcTileset &sgcTileset) const
{
    if(m_currentSymbolIndex >= m_tileSymbols.size())
        return std::nullopt;

    auto position = m_tileSymbols[m_currentSymbolIndex];

    if(sgcTileset.IsValid(position))
    {
        return m_tileSymbols[m_currentSymbolIndex];
    }
    
    return std::nullopt;
}

std::optional<sgc::tile::TilePosition2D> program::TileSymbolGroup::NextSymbol(const SgcTileset &sgcTileset)
{
    auto returnValue = PeekSymbol(sgcTileset);
    m_currentSymbolIndex = (m_currentSymbolIndex + 1) % m_tileSymbols.size();
    return returnValue;
}

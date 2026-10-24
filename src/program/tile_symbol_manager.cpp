#include "program/tile_symbol_manager.hpp"

void program::TileSymbolManager::CreateSymbolGroup(std::wstring groupName, SgcTileset &sgcTileset, const sgc::tile::TileSize2D &symbolSize, const std::vector<sgc::tile::TilePosition2D> &positions)
{
    m_tileSymbolGroups.push_back({groupName, {sgcTileset}});

    auto &newGroup = m_tileSymbolGroups.back();

    newGroup.group.SetSize(symbolSize);

    for(auto &position : positions){
        newGroup.group.AddSymbol(position);
    }

    return;
}

program::TileSymbolGroup* program::TileSymbolManager::PeekSymbolGroup()
{
    return &(m_tileSymbolGroups[m_currentGroupIndex].group);
}

program::TileSymbolGroup* program::TileSymbolManager::NextSymbolGroup()
{
    auto returnValue = PeekSymbolGroup();
    m_currentGroupIndex = (m_currentGroupIndex + 1) % m_tileSymbolGroups.size();
    return returnValue;
}
#include "program/tile_symbol_manager.hpp"

void program::TileSymbolManager::CreateSymbolGroup(std::wstring groupName, const sgc::tile::TileSize2D &symbolSize, const std::vector<sgc::tile::TilePosition2D> &positions)
{
    m_tileSymbolGroups.push_back({groupName, {}});

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

std::vector<program::TileSymbolEntry> program::TileSymbolManager::ClearSymbolGroups()
{
    auto returnValue = m_tileSymbolGroups;
    m_tileSymbolGroups.clear();
    m_currentGroupIndex = 0;
    return returnValue;
}

bool program::TileSymbolManager::IsEmpty() const
{
    return m_tileSymbolGroups.empty();
}

std::vector<program::TileSymbolEntry> program::TileSymbolManager::GetSymbolGroups() const
{
    return m_tileSymbolGroups;
}

void program::TileSymbolManager::LoadSymbolGroups(const file::SymbolGroupsInfo &symbolGroupsInfo)
{
    for(auto &groupInfo : symbolGroupsInfo.groups) 
    {
        CreateSymbolGroup(groupInfo.groupName, groupInfo.symbolSize, groupInfo.positions);
    }
}
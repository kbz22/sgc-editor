#include "command/paint_command.hpp"

command::PaintCommand::PaintCommand(file::MapDocument* mapDocument, size_t activeLayerIndex) :
    m_mapDocument{mapDocument},
    m_activeLayerIndex{activeLayerIndex}
{}

void command::PaintCommand::ExecuteTileChange(const TileChange &tileChange)
{
    if(m_tileChanges.find(tileChange.position) == m_tileChanges.end()) {
        m_tileChanges[tileChange.position] = tileChange;
    }
    else {
        auto &existingTileChange = m_tileChanges[tileChange.position];
        existingTileChange.newTileId = tileChange.newTileId;
    }

    auto layerManager = m_mapDocument->GetLayerManager();
    layerManager->GetLayers()[m_activeLayerIndex].storage->SetTileAt(tileChange.position, tileChange.newTileId);    
}

void command::PaintCommand::UndoTileChanges()
{
    if(m_mapDocument == nullptr) {
        return;
    }
    auto layerManager = m_mapDocument->GetLayerManager();
    for (const auto& [position, change] : m_tileChanges) {
        layerManager->GetLayers()[m_activeLayerIndex].storage->SetTileAt(position, change.previousTileId);
    }
}

void command::PaintCommand::Execute()
{
    if(m_mapDocument == nullptr) {
        return;
    }

    auto layerManager = m_mapDocument->GetLayerManager();
    for (const auto& [position, change] : m_tileChanges) {        
        layerManager->GetLayers()[m_activeLayerIndex].storage->SetTileAt(position, change.newTileId);
    }
    m_mapDocument->SetDirty(true);
}

void command::PaintCommand::Commit()
{
    m_mapDocument->SetDirty(true);
}

void command::PaintCommand::Undo()
{
    UndoTileChanges();
    m_mapDocument->SetDirty(true);
}
#include "command/paint_command.hpp"

command::PaintCommand::PaintCommand(file::MapDocument* mapDocument, size_t activeLayerIndex) :
    m_mapDocument{mapDocument},
    m_activeLayerIndex{activeLayerIndex}
{}

void command::PaintCommand::ExecuteTileChange(const TileChange &tileChange)
{
    m_tileChanges.insert_or_assign(tileChange.position, tileChange);

    auto layerManager = m_mapDocument->GetLayerManager();
    layerManager->GetLayers()[m_activeLayerIndex].storage->SetTileAt(tileChange.position, tileChange.newTileId);
}

void command::PaintCommand::ExecuteTileChange(const std::vector<TileChange> &tileChanges)
{
    m_tileChanges.reserve(m_tileChanges.size() + tileChanges.size());
    auto layerManager = m_mapDocument->GetLayerManager();

    for (const auto& change : tileChanges) {
        m_tileChanges.insert_or_assign(change.position, change);
        layerManager->GetLayers()[m_activeLayerIndex].storage->SetTileAt(change.position, change.newTileId);
    }
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
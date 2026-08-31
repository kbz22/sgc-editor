#include "command/paint_command.hpp"

command::PaintCommand::PaintCommand(file::MapDocument* mapDocument, size_t activeLayerIndex) :
    m_mapDocument{mapDocument},
    m_activeLayerIndex{activeLayerIndex}
{}

command::PaintCommand::PaintCommand(const PaintCommand& other) = default;

void command::PaintCommand::ExecuteTileChange(const TileChange &tileChange)
{
    if(m_tileChanges.contains(tileChange.position)) {
        TileChange& existingChange = m_tileChanges[tileChange.position];
        existingChange.newTileId = tileChange.newTileId;            
    }
    else {
        m_tileChanges[tileChange.position] = tileChange;
    }

    auto layerManager = m_mapDocument->GetLayerManager();
    layerManager->GetLayers()[m_activeLayerIndex].storage->SetTileAt(tileChange.position, tileChange.newTileId);
}

void command::PaintCommand::ExecuteTileChange(const std::unordered_map<sgc::math::vec2, TileChange> &tileChanges)
{
    m_tileChanges.reserve(m_tileChanges.size() + tileChanges.size());
    auto layerManager = m_mapDocument->GetLayerManager();

    for (const auto& change : tileChanges) {
        if(m_tileChanges.contains(change.first)) {
            TileChange& existingChange = m_tileChanges[change.first];
            existingChange.newTileId = change.second.newTileId;            
        }
        else {
            m_tileChanges[change.first] = change.second;
        }
        layerManager->GetLayers()[m_activeLayerIndex].storage->SetTileAt(change.first, change.second.newTileId);
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
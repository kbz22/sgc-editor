#include "command/paint_command.hpp"
#include "program/program.hpp"

command::PaintCommand::PaintCommand(size_t activeLayerIndex) :
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

    auto mapDocument = program::GetProgramContext().fileManager->GetSelectedDocument();
    auto layerManager = mapDocument->GetLayerManager();
    layerManager->GetLayers()[m_activeLayerIndex].storage->SetTileAt(tileChange.position, tileChange.newTileId);
    mapDocument->SetDirty(true);
}

void command::PaintCommand::Execute()
{
    auto &programContext = program::GetProgramContext();
    auto mapDocument = programContext.fileManager->GetSelectedDocument();

    if(mapDocument == nullptr) {
        return;
    }

    auto layerManager = mapDocument->GetLayerManager();
    for (const auto& [position, change] : m_tileChanges) {        
        layerManager->GetLayers()[m_activeLayerIndex].storage->SetTileAt(position, change.newTileId);
    }
    mapDocument->SetDirty(true);
}

void command::PaintCommand::Undo()
{
    auto mapDocument = program::GetProgramContext().fileManager->GetSelectedDocument();
    auto layerManager = mapDocument->GetLayerManager();
    for (const auto& [position, change] : m_tileChanges) {
        // m_tileStorage.SetTileAt(position, change.previousTileId);
        layerManager->GetLayers()[m_activeLayerIndex].storage->SetTileAt(position, change.previousTileId);
    }
    mapDocument->SetDirty(true);    
}
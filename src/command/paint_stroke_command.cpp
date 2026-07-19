#include "command/paint_stroke_command.hpp"
#include "program/program.hpp"

command::PaintStrokeCommand::PaintStrokeCommand(size_t activeLayerIndex) :
    m_activeLayerIndex{activeLayerIndex}
{}

void command::PaintStrokeCommand::ExecuteTileChange(const TileChange &tileChange)
{
    if(m_tileChanges.find(tileChange.position) == m_tileChanges.end()) {
        m_tileChanges[tileChange.position] = tileChange;
    }
    else {
        auto &existingTileChange = m_tileChanges[tileChange.position];
        existingTileChange.newTileId = tileChange.newTileId;
    }

    // m_tileStorage.SetTileAt(tileChange.position, tileChange.newTileId);
    auto &layerManager = program::GetProgramContext().layerManager;
    layerManager->GetLayers()[m_activeLayerIndex].storage->SetTileAt(tileChange.position, tileChange.newTileId);
}

void command::PaintStrokeCommand::Execute()
{
    auto &layerManager = program::GetProgramContext().layerManager;
    for (const auto& [position, change] : m_tileChanges) {
        //   m_tileStorage.SetTileAt(position, change.newTileId);
        layerManager->GetLayers()[m_activeLayerIndex].storage->SetTileAt(position, change.newTileId);
    }
}

void command::PaintStrokeCommand::Undo()
{
    auto &layerManager = program::GetProgramContext().layerManager;
    for (const auto& [position, change] : m_tileChanges) {
        // m_tileStorage.SetTileAt(position, change.previousTileId);
        layerManager->GetLayers()[m_activeLayerIndex].storage->SetTileAt(position, change.previousTileId);    }
}
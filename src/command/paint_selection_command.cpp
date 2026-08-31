#include "command/paint_selection_command.hpp"

command::PaintSelectionCommand::PaintSelectionCommand(file::MapDocument& mapDocument, const MultilayerTileChangesType& tileChanges) :
    m_mapDocument(mapDocument),
    m_tileChanges(tileChanges)
{}

command::PaintSelectionCommand::PaintSelectionCommand(const PaintSelectionCommand& other) = default;

void command::PaintSelectionCommand::Execute()
{
    auto layerManager = m_mapDocument.GetLayerManager();
    auto layers = layerManager->GetLayers();    

    for (const auto& [layerIndex, tileChanges] : m_tileChanges)
    {        
        for (const auto& [tilePosition, tileChange] : tileChanges)
        {
            layers[layerIndex].storage->SetTileAt(tilePosition, tileChange.newTileId);
        }
    }

    m_mapDocument.SetDirty(true);
}

void command::PaintSelectionCommand::Commit()
{
    return;
}

void command::PaintSelectionCommand::Undo()
{
    auto layerManager = m_mapDocument.GetLayerManager();
    auto layers = layerManager->GetLayers();    

    for (const auto& [layerIndex, tileChanges] : m_tileChanges)
    {        
        for (const auto& [tilePosition, tileChange] : tileChanges)
        {
            layers[layerIndex].storage->SetTileAt(tilePosition, tileChange.oldTileId);
        }
    }

    m_mapDocument.SetDirty(true);
}
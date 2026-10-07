#include "editor_tools/tile_picker_tool.hpp"

editor_tools::TilePickerTool::TilePickerTool(sections::TilesetSection& tilesetSection) :
    m_tilesetSection{tilesetSection}
{}

void editor_tools::TilePickerTool::Execute(file::MapDocument& mapDocument, sgc::tile::TilePosition2D cursorPosition)
{
    auto layerManager = mapDocument.GetLayerManager();
    auto currentLayer = layerManager->GetLayers()[layerManager->GetActiveLayerIndex()];
    auto selectedTileId = currentLayer.storage->GetTileAt(cursorPosition);

    if(selectedTileId.has_value())
    {
        m_tilesetSection.SetCursorTileId(selectedTileId.value());
        m_tilesetSection.Update();
    }
}

void editor_tools::TilePickerTool::Commit([[maybe_unused]]file::MapDocument& mapDocument)
{
    // No commit action needed for tile picker tool
    return;
}
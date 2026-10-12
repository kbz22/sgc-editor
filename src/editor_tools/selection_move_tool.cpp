#include "editor_tools/selection_move_tool.hpp"
#include "command/paint_selection_command.hpp"
#include "sections/map_section.hpp"

editor_tools::SelectionMoveTool::SelectionMoveTool(sgc_view::MapView &mapView, sections::MapSection &mapSection, SelectionMode &selectionMode) :
    m_mapView{mapView},
    m_mapSection{mapSection},
    m_selectionMode{selectionMode}
{}

void editor_tools::SelectionMoveTool::SelectionMoveTool::Execute(file::MapDocument& mapDocument, sgc::tile::TilePosition2D cursorPosition)
{
    auto selectionPosition = m_mapView.GetSelectionPositionInTiles();
    auto selectionSize = m_mapView.GetSelectionSizeInTiles();

    if(selectionSize.x == 0 || selectionSize.y == 0)
        return;

    if(m_movingSelectionOffset == nullptr)
    {
        m_movingSelectionOffset = std::make_unique<sgc::tile::TilePosition2D>(
            cursorPosition.x - selectionPosition.x,
            cursorPosition.y - selectionPosition.y
        );
    }        

    auto iterateTiles = [&selectionPosition, &selectionSize, this](program::LayerItem &layerItem, size_t renderingIndex, size_t storageIndex)
    {
        auto selectionStorage = std::make_shared<sgc::data::StaticTileStorage>(selectionSize);
        for(sgc::math::ival y = 0; y < selectionSize.y; ++y) {
            for(sgc::math::ival x = 0; x < selectionSize.x; ++x) 
            {
                auto tilePos = sgc::tile::TilePosition2D{
                    selectionPosition.x + x,
                    selectionPosition.y + y
                };

                auto tileId = layerItem.storage->GetTileAt(tilePos);
                selectionStorage->SetTileAt({x, y}, tileId);
            }
        }

        (*m_selectionMovedStorage)[storageIndex].storage = selectionStorage;
        (*m_selectionMovedStorage)[storageIndex].transparency = layerItem.transparency;
        (*m_selectionMovedStorage)[storageIndex].renderingIndex = renderingIndex;
    };

    if(m_selectionMovedStorage == nullptr)
    {
        m_selectionMovedStorage = std::make_unique<program::SelectionMovedStorage>();

        auto layerManager = mapDocument.GetLayerManager();
        auto layers = layerManager->GetLayers();
        auto activeLayerIndex = layerManager->GetActiveLayerIndex();

        switch(m_selectionMode)
        {
            case editor_tools::SelectionMode::SingleLayer:
            {                            
                iterateTiles(layers[activeLayerIndex], layers.size() - activeLayerIndex - 1, activeLayerIndex);
                break;
            }                        

            case editor_tools::SelectionMode::AllLayers:
            {
                for(size_t i = 0; i < layers.size(); i++) {
                    iterateTiles(layers[i], layers.size() - i - 1, i);
                }
                break;
            }

            case editor_tools::SelectionMode::VisibleLayers:
            {
                for(size_t i = 0; i < layers.size(); i++) {
                    if(layers[i].visible) {
                        iterateTiles(layers[i], layers.size() - i - 1, i);
                    }
                }
                break;
            }
        }

        m_mapSection.SetSelectionMovedImage(*m_selectionMovedStorage);
    }

    m_mapSection.UpdateSelectionMovedPosition(selectionPosition - *m_movingSelectionOffset);    
}

void editor_tools::SelectionMoveTool::SelectionMoveTool::Commit(file::MapDocument& mapDocument)
{
    auto selectionLayers = m_mapView.GetSelectionLayers();
    auto selectionPos = m_mapView.GetSelectionPositionInTiles();
    auto selectionSize = m_mapView.GetSelectionSizeInTiles();
    auto layerManager = mapDocument.GetLayerManager();
    auto layers = layerManager->GetLayers();
    auto clearTileId = m_mapView.GetClearTileId();
    command::MultilayerTileChangesType tileChanges;

    for(auto &[storageIndex, storageLayer] : *m_selectionMovedStorage)
    {
        for(sgc::math::ival y = 0; y < selectionSize.y; ++y) {
            for(sgc::math::ival x = 0; x < selectionSize.x; ++x)
            {
                auto tilePos = sgc::tile::TilePosition2D{
                    selectionPos.x + x,
                    selectionPos.y + y
                };

                auto movedTileId = storageLayer.storage->GetTileAt({x, y});
                auto currentTileId = layers[storageIndex].storage->GetTileAt(tilePos);
                
                tileChanges[storageIndex][tilePos] = {
                    storageIndex,
                    tilePos,
                    currentTileId,
                    movedTileId.value_or(currentTileId.value_or(clearTileId))
                };
            }
        }
    }

    mapDocument.GetCommandManager()->Execute(std::make_unique<command::PaintSelectionCommand>(
        mapDocument,
        tileChanges
    ));

    selectionLayers->Clear();
    m_selectionMovedStorage.reset();
}
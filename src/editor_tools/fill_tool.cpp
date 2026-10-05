#include "editor_tools/fill_tool.hpp"
#include "editor_tools/helpers.hpp"
#include <unordered_set>
#include <stack>

editor_tools::FillTool::FillTool(sgc_view::MapView &mapView, sections::TilesetSection &tilesetSection, const bool &allowChunkCreation) :
    Brush{mapView, tilesetSection, allowChunkCreation}
{}

void editor_tools::FillTool::Execute(file::MapDocument& mapDocument, sgc::tile::TilePosition2D cursorPosition)
{
    auto currentLayer = mapDocument.GetCurrentLayerStorage();

    if(currentLayer == nullptr) {
        return;
    }

    if(m_paintCommand == nullptr){
        m_paintCommand = std::make_unique<command::PaintCommand>(
            &mapDocument,
            mapDocument.GetLayerManager()->GetActiveLayerIndex()
        );
        m_cursorOrigin = std::make_unique<sgc::tile::TilePosition2D>(
            cursorPosition.x,
            cursorPosition.y
        );
    }

    auto targetTileId = currentLayer->GetTileAt(cursorPosition);
    auto clearTileId = m_tilesetSection.GetClearTileId();
    auto positionOnTilesetOp = m_tilesetSection.GetCursorPositionInPixels();

    if(!positionOnTilesetOp.has_value())
        return; 

    if(m_allowChunkCreation && !targetTileId.has_value()) 
    {
        return;
    }
    else 
    {
        if(targetTileId == std::nullopt) 
        {
            targetTileId = clearTileId;

            auto& chunkedStorage = *dynamic_cast<sgc::data::ChunkedTileStorage*>(currentLayer);
            chunkedStorage.SetChunkAt(
                chunkedStorage.GetChunkCoordAt(cursorPosition),
                clearTileId
            );
        }

        currentLayer->SetTileAt(cursorPosition, targetTileId);
    }

    std::vector<sgc::tile::TilePosition2D> region;
    std::unordered_set<sgc::tile::TilePosition2D> visited;

    std::stack<sgc::tile::TilePosition2D> stack;
    stack.push(cursorPosition);

    while (!stack.empty())
    {
        auto pos = stack.top();
        stack.pop();

        if (visited.contains(pos))
            continue;

        visited.insert(pos);

        auto tile = currentLayer->GetTileAt(pos);

        if (tile != targetTileId)
            continue;

        region.push_back(pos);

        stack.push({pos.x + 1, pos.y});
        stack.push({pos.x - 1, pos.y});
        stack.push({pos.x, pos.y + 1});
        stack.push({pos.x, pos.y - 1});
    }

    auto tileset = m_mapView.GetSgcTileset();
    auto cursorPositionOnTileset = positionOnTilesetOp.value();
    auto tileSize = m_mapView.GetTileSize();
    auto cursorSize = m_mapView.GetCursorSizeInTiles();

    cursorPositionOnTileset.x /= tileSize.x;
    cursorPositionOnTileset.y /= tileSize.y;

    for(auto pos : region)
    {
        auto newTileId = GetTileId(
            tileset,
            cursorPositionOnTileset,
            cursorSize,
            pos,
            *m_cursorOrigin
        );

        command::TileChange change{
            pos,
            currentLayer->GetTileAt(pos),
            newTileId
        };

        m_paintCommand->ExecuteTileChange(change);
    }
}
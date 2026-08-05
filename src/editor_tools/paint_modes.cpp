#include "editor_tools/brush.hpp"
#include <stack>
#include <unordered_set>
#include <optional>
#include <sgc/math/value.hpp>
#include <sgc/data/chunkedtilestorage.hpp>

std::optional<sgc::tile::TileId> GetTileId(
    sgc::graphics::Tileset& tileset,
    sgc::tile::TilePosition2D cursorPositionOnTileset,
    sgc::tile::TileSize2D tileSize,
    sgc::tile::TilePosition2D tilePosition,
    sgc::tile::TilePosition2D selectionStart
)
{
    auto deltaX = sgc::math::AbsMod(
        tilePosition.x - selectionStart.x,
        tileSize.x
    );

    auto deltaY = sgc::math::AbsMod(
        tilePosition.y - selectionStart.y,
        tileSize.y
    );

    return tileset.ToTileId(
        cursorPositionOnTileset.x + deltaX,
        cursorPositionOnTileset.y + deltaY
    );
}


void editor_tools::PaintStroke(
    Brush& brush,
    file::MapDocument& mapDocument,
    sgc::graphics::Tileset& tileset,
    sgc::tile::TilePosition2D tilePosition,
    sgc::tile::TilePosition2D cursorPositionOnTileset,
    sgc::tile::TileSize2D tileSize
)
{
    auto currentLayer = mapDocument.GetCurrentLayerStorage();

    if(currentLayer == nullptr) {
        return;
    }

    for(auto x = 0; x < tileSize.x; ++x){
        for(auto y = 0; y < tileSize.y; ++y)
        {
            auto tileMapX = tilePosition.x + x;
            auto tileMapY = tilePosition.y + y;

            auto tileId = GetTileId(
                tileset,
                cursorPositionOnTileset,
                tileSize,
                { tileMapX, tileMapY },
                *brush.m_selectionStart
            );

            auto hasValue = currentLayer->GetTileAt({ tileMapX, tileMapY }).has_value();

            if( !brush.m_checkTileBeforePainting || hasValue) {

                if(!hasValue){
                    auto chunkStorage = dynamic_cast<sgc::data::ChunkedTileStorage*>(currentLayer);
                    chunkStorage->SetChunkAt(
                        chunkStorage->GetChunkCoordAt({ tileMapX, tileMapY }),
                        brush.m_clearTileId
                    );
                }

                command::TileChange change{
                    { tileMapX, tileMapY },
                    currentLayer->GetTileAt({ tileMapX, tileMapY }),
                    tileId
                };
                
                brush.m_paintCommand->ExecuteTileChange(change);

            } 
        }
    }
}

void editor_tools::PaintRectangle(
    Brush& brush,
    file::MapDocument& mapDocument,
    sgc::graphics::Tileset& tileset,
    sgc::tile::TilePosition2D tilePosition,
    sgc::tile::TilePosition2D cursorPositionOnTileset,
    sgc::tile::TileSize2D tileSize
)
{
    auto currentLayer = mapDocument.GetCurrentLayerStorage();

    if(currentLayer == nullptr) {
        return;
    }

    brush.m_paintCommand->UndoTileChanges();

    auto rectStartX = std::min(brush.m_selectionStart->x, tilePosition.x);
    auto rectEndX = std::max(brush.m_selectionStart->x, tilePosition.x);
    auto rectStartY = std::min(brush.m_selectionStart->y, tilePosition.y);
    auto rectEndY = std::max(brush.m_selectionStart->y, tilePosition.y);

    for(auto x = rectStartX; x <= rectEndX; ++x){
        for(auto y = rectStartY; y <= rectEndY; ++y)
        {
            auto tileMapX = x;
            auto tileMapY = y;

            auto tileId = GetTileId(
                tileset,
                cursorPositionOnTileset,
                tileSize,
                { tileMapX, tileMapY },
                *brush.m_selectionStart
            );

            auto getTile = currentLayer->GetTileAt({ tileMapX, tileMapY });

            if( !brush.m_checkTileBeforePainting || getTile.has_value()) {

                if(!getTile.has_value()){
                    auto chunkStorage = dynamic_cast<sgc::data::ChunkedTileStorage*>(currentLayer);
                    chunkStorage->SetChunkAt(
                        chunkStorage->GetChunkCoordAt({ tileMapX, tileMapY }),
                        brush.m_clearTileId
                    );
                }

                command::TileChange change{
                    { tileMapX, tileMapY },
                    getTile,
                    tileId
                };
                
                brush.m_paintCommand->ExecuteTileChange(change);

            } 
        }
    }
}

void editor_tools::PaintFill(
    Brush& brush,
    file::MapDocument& mapDocument,
    sgc::graphics::Tileset& tileset,
    sgc::tile::TilePosition2D tilePosition,
    sgc::tile::TilePosition2D cursorPositionOnTileset,
    sgc::tile::TileSize2D tileSize
)
{
    auto currentLayer = mapDocument.GetCurrentLayerStorage();

    if(currentLayer == nullptr) {
        return;
    }

    auto targetTileId = currentLayer->GetTileAt(tilePosition);

    if(brush.m_checkTileBeforePainting && !targetTileId.has_value()) {
        return;
    }
    else {
        if(targetTileId == std::nullopt) {
            targetTileId = brush.m_clearTileId;
        }

        currentLayer->SetTileAt(tilePosition, targetTileId);
    }

    std::vector<sgc::tile::TilePosition2D> region;
    std::unordered_set<sgc::tile::TilePosition2D> visited;

    std::stack<sgc::tile::TilePosition2D> stack;
    stack.push(tilePosition);

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

    for(auto pos : region)
    {
        auto newTileId = GetTileId(
            tileset,
            cursorPositionOnTileset,
            tileSize,
            pos,
            *brush.m_selectionStart
        );

        command::TileChange change{
            pos,
            currentLayer->GetTileAt(pos),
            newTileId
        };

        brush.m_paintCommand->ExecuteTileChange(change);
    }

}
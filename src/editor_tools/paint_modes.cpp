#include "editor_tools/brush.hpp"
#include <stack>
#include <unordered_set>
#include <optional>
#include <sgc/math/value.hpp>
#include <sgc/data/chunkedtilestorage.hpp>

inline std::optional<sgc::tile::TileId> GetTileId(
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

inline std::vector<command::TileChange> CreateTileChanges(
    sgc::math::ival x_start,
    sgc::math::ival x_end,
    sgc::math::ival y_start,
    sgc::math::ival y_end,
    sgc::graphics::Tileset& tileset,
    sgc::tile::TilePosition2D cursorPositionOnTileset,
    sgc::tile::TileSize2D tileSize,
    sgc::tile::TilePosition2D selectionStart,
    sgc::data::ITileStorage* currentLayer,
    sgc::tile::TileId clearTileId,
    bool checkTileBeforePainting,
    bool erase
)
{
    std::vector<command::TileChange> tileChanges;
    auto size_x = sgc::math::Abs(x_end - x_start) + 1;
    auto size_y = sgc::math::Abs(y_end - y_start) + 1;
    tileChanges.reserve(size_x * size_y);

    for(auto x = x_start; x <= x_end; ++x){
        for(auto y = y_start; y <= y_end; ++y)
        {
            auto tileId = GetTileId(
                tileset,
                cursorPositionOnTileset,
                tileSize,
                { x, y },
                selectionStart
            );
            
            auto checkTile = currentLayer->GetTileAt({ x, y });

            if( !checkTileBeforePainting || checkTile.has_value()) {

                if(!checkTile.has_value()){
                    auto chunkStorage = dynamic_cast<sgc::data::ChunkedTileStorage*>(currentLayer);
                    chunkStorage->SetChunkAt(
                        chunkStorage->GetChunkCoordAt({ x, y }),
                        clearTileId
                    );
                }            

                command::TileChange change{
                    { x, y },
                    checkTile,
                    erase ? clearTileId : tileId
                };
                
                tileChanges.push_back(change);
            }
        }        
    }

    return tileChanges;
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

    std::vector<command::TileChange> tileChanges;

    tileChanges = CreateTileChanges(
        tilePosition.x,
        tilePosition.x + tileSize.x - 1,
        tilePosition.y,
        tilePosition.y + tileSize.y - 1,
        tileset,
        cursorPositionOnTileset,
        tileSize,
        *brush.m_selectionStart,
        currentLayer,
        brush.m_clearTileId,
        brush.m_checkTileBeforePainting,
        brush.m_eraserMode == EraserMode::ClearTile
    );
    
    if(!tileChanges.empty()) {
        brush.m_paintCommand->ExecuteTileChange(tileChanges);
        return;
    }
}

void editor_tools::EraseStroke(
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
    
    auto tileChanges = CreateTileChanges(
        tilePosition.x,
        tilePosition.x + tileSize.x - 1,
        tilePosition.y,
        tilePosition.y + tileSize.y - 1,
        tileset,
        cursorPositionOnTileset,
        tileSize,
        *brush.m_selectionStart,
        currentLayer,
        brush.m_clearTileId,
        brush.m_checkTileBeforePainting,
        true
    );

    if(!tileChanges.empty()) {
        brush.m_paintCommand->ExecuteTileChange(tileChanges);
        return;
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

    auto tileChanges = CreateTileChanges(
        rectStartX,
        rectEndX,
        rectStartY,
        rectEndY,
        tileset,
        cursorPositionOnTileset,
        tileSize,
        *brush.m_selectionStart,
        currentLayer,
        brush.m_clearTileId,
        brush.m_checkTileBeforePainting,
        brush.m_eraserMode == EraserMode::ClearTile
    );

    if(!tileChanges.empty()) {
        brush.m_paintCommand->ExecuteTileChange(tileChanges);
        return;
    }    
}

void editor_tools::EraseRectangle(
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

    auto tileChanges = CreateTileChanges(
        rectStartX,
        rectEndX,
        rectStartY,
        rectEndY,
        tileset,
        cursorPositionOnTileset,
        tileSize,
        *brush.m_selectionStart,
        currentLayer,
        brush.m_clearTileId,
        brush.m_checkTileBeforePainting,
        true
    );

    if(!tileChanges.empty()) {
        brush.m_paintCommand->ExecuteTileChange(tileChanges);
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
        // i assume chunked storage here
        // which for now is true for all layers
        //!but in the future this might need a check (dynamic should throw if not chunked)
        if(targetTileId == std::nullopt) {
            targetTileId = brush.m_clearTileId;

            auto& chunkedStorage = *dynamic_cast<sgc::data::ChunkedTileStorage*>(currentLayer);
            chunkedStorage.SetChunkAt(
                chunkedStorage.GetChunkCoordAt(tilePosition),
                brush.m_clearTileId
            );
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

void editor_tools::EraseFill(
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
        command::TileChange change{
            pos,
            currentLayer->GetTileAt(pos),
            brush.m_clearTileId
        };

        brush.m_paintCommand->ExecuteTileChange(change);
    }
}
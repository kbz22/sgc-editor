#include "editor_tools/paint_brush.hpp"
#include "editor_tools/helpers.hpp"
#include "command/create_chunk_command.hpp"

editor_tools::PaintBrush::PaintBrush(sgc_view::MapView &mapView, sections::TilesetSection &tilesetSection, const bool &allowChunkCreation, bool &needsRedraw) :
    Brush{mapView, tilesetSection, allowChunkCreation, needsRedraw}
{}

void editor_tools::PaintBrush::Execute(file::MapDocument& mapDocument, sgc::tile::TilePosition2D cursorPosition)
{
    auto currentLayer = mapDocument.GetCurrentLayerStorage();

    if(currentLayer == nullptr) {
        return;
    }

    if(m_paintCommand == nullptr)
    {
        m_paintCommand = std::make_unique<command::PaintCommand>(
            &mapDocument,
            mapDocument.GetLayerManager()->GetActiveLayerIndex()
        );

        m_bulkCommand.reset();
        m_bulkCommand = std::make_unique<command::BulkCommand>();

        m_cursorOrigin = std::make_unique<sgc::tile::TilePosition2D>(
            cursorPosition.x,
            cursorPosition.y
        );
    }

    auto positionOnTilesetOp = m_tilesetSection.GetCursorPositionInPixels();

    if(!positionOnTilesetOp.has_value())
        return;    

    std::unordered_map<sgc::math::vec2, command::TileChange> tileChanges;
    auto cursorSize = m_mapView.GetCursorSizeInTiles();
    auto tileSize = m_mapView.GetTileSize();
    auto positionOnTileset = positionOnTilesetOp.value();

    positionOnTileset.x /= tileSize.x;
    positionOnTileset.y /= tileSize.y;

    auto endPosition = sgc::tile::TilePosition2D{
        cursorPosition.x + cursorSize.x - 1,
        cursorPosition.y + cursorSize.y - 1
    };   

    for(auto x = cursorPosition.x; x <= endPosition.x; ++x){
        for(auto y = cursorPosition.y; y <= endPosition.y; ++y)
        {
            auto tileId = GetTileId(
                // *m_mapView.GetTileset(),
                m_mapView.GetSgcTileset(),
                positionOnTileset,
                cursorSize,
                { x, y },
                *m_cursorOrigin
            );
            
            auto checkTile = currentLayer->GetTileAt({ x, y });

            if( !m_allowChunkCreation || checkTile.has_value()) {

                if(!checkTile.has_value()){
                    auto chunkStorage = dynamic_cast<sgc::data::ChunkedTileStorage*>(currentLayer);
                    auto chunkCoord = chunkStorage->GetChunkCoordAt({ x, y });
                    auto createChunkCommand = std::make_unique<command::CreateChunkCommand>(chunkCoord);

                    createChunkCommand->Execute();
                    checkTile = currentLayer->GetTileAt({ x, y });
                    m_bulkCommand->AddCommand(std::move(createChunkCommand));                    
                }

                command::TileChange change{
                    { x, y },
                    checkTile,
                    tileId
                };
                
                tileChanges[{ x, y }] = change;
            }
        }        
    }
    
    if(!tileChanges.empty()) {
        m_paintCommand->ExecuteTileChange(tileChanges);
        m_needsRedraw = true;
        return;
    }
}

#include "editor_tools/rectangular_brush.hpp"
#include "editor_tools/helpers.hpp"

editor_tools::RectangularBrush::RectangularBrush(sgc_view::MapView &mapView, sections::TilesetSection &tilesetSection, const bool &allowChunkCreation, bool &needsRedraw) :
    Brush{mapView, tilesetSection, allowChunkCreation, needsRedraw}
{}

void editor_tools::RectangularBrush::Execute(file::MapDocument& mapDocument, sgc::tile::TilePosition2D cursorPosition)
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
    else{
        m_paintCommand->UndoTileChanges();
    }

    std::unordered_map<sgc::math::vec2, command::TileChange> tileChanges;
    auto positionOnTileset = m_tilesetSection.GetCursorPositionInTiles();
    auto cursorSize = m_tilesetSection.GetCursorSizeInTiles();    

    auto startPosition = sgc::tile::TilePosition2D{
        cursorPosition.x > m_cursorOrigin->x ? m_cursorOrigin->x : cursorPosition.x,
        cursorPosition.y > m_cursorOrigin->y ? m_cursorOrigin->y : cursorPosition.y,
    };

    auto endPosition = sgc::tile::TilePosition2D{
        cursorPosition.x <= m_cursorOrigin->x ? m_cursorOrigin->x : cursorPosition.x,
        cursorPosition.y <= m_cursorOrigin->y ? m_cursorOrigin->y : cursorPosition.y,
    };

    for(auto x = startPosition.x; x <= endPosition.x; ++x){
        for(auto y = startPosition.y; y <= endPosition.y; ++y)
        {
            auto tileId = GetTileId(
                m_mapView.GetSgcTileset(),
                positionOnTileset,
                cursorSize,
                { x, y },
                *m_cursorOrigin
            );
            
            auto checkTile = currentLayer->GetTileAt({ x, y });

            if( m_allowChunkCreation || checkTile.has_value()) {

                if(!checkTile.has_value()){
                    auto chunkStorage = dynamic_cast<sgc::data::ChunkedTileStorage*>(currentLayer);
                    auto chunkCoord = chunkStorage->GetChunkCoordAt({ x, y });

                    chunkStorage->SetChunkAt(
                        chunkCoord,
                        m_tilesetSection.GetClearTileId()
                    );

                    for(int __x = 0; __x < sgc::data::TileChunk::Size; ++__x){
                        for(int __y = 0; __y < sgc::data::TileChunk::Size; ++__y){
                            auto localX = chunkCoord.x * sgc::data::TileChunk::Size + __x;
                            auto localY = chunkCoord.y * sgc::data::TileChunk::Size + __y;

                            if(tileChanges.contains({ localX, localY })) {
                                continue;
                            }

                            command::TileChange change{
                                { localX, localY },
                                checkTile,
                                m_tilesetSection.GetClearTileId()
                            };
                            
                            tileChanges[{ localX, localY }] = change;
                        }
                    }
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
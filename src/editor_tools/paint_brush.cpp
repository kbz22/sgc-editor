#include "editor_tools/paint_brush.hpp"
#include "editor_tools/helpers.hpp"

editor_tools::PaintBrush::PaintBrush(sgc_view::MapView &mapView, sections::TilesetSection &tilesetSection, const bool &allowChunkCreation) :
    m_paintCommand{nullptr},
    m_cursorOrigin{nullptr},
    m_mapView{mapView},
    m_tilesetSection{tilesetSection},
    m_allowChunkCreation{allowChunkCreation}
{}

void editor_tools::PaintBrush::Execute(file::MapDocument& mapDocument, sgc::tile::TilePosition2D cursorPosition)
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

    auto positionOnTilesetOp = m_tilesetSection.GetCursorPositionInPixels();

    if(!positionOnTilesetOp.has_value())
        return;    

    std::unordered_map<sgc::math::vec2, command::TileChange> tileChanges;
    auto cursorSize = m_mapView.GetCursorSizeInTiles();
    auto tileSize = m_mapView.GetTileSize();
    auto positionOnTileset = positionOnTilesetOp.value();

    auto endPosition = sgc::tile::TilePosition2D{
        cursorPosition.x + cursorSize.x - 1,
        cursorPosition.y + cursorSize.y - 1
    };

    positionOnTileset.x /= tileSize.x;
    positionOnTileset.y /= tileSize.y;

    for(auto x = cursorPosition.x; x <= endPosition.x; ++x){
        for(auto y = cursorPosition.y; y <= endPosition.y; ++y)
        {
            auto tileId = GetTileId(
                *m_mapView.GetTileset(),
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
        return;
    }
}

void editor_tools::PaintBrush::Commit(file::MapDocument& mapDocument)
{
    auto commandManager = mapDocument.GetCommandManager();

    if(m_paintCommand != nullptr) {
        commandManager->Commit(std::move(m_paintCommand));
    }

    m_paintCommand.reset();
    m_cursorOrigin.reset();
}
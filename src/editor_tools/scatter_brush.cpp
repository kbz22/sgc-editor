#include "editor_tools/scatter_brush.hpp"
#include "command/create_chunk_command.hpp"
#include <algorithm>
#include <cstdint>
#include <vector>
#include <random>

editor_tools::ScatterBrush::ScatterBrush(sgc_view::MapView &mapView, sections::TilesetSection &tilesetSection, const bool &allowChunkCreation, bool &needsRedraw, sgc::tile::TileId &clearTileId, sgc::math::ival radiusValue) :
    Brush{mapView, tilesetSection, allowChunkCreation, needsRedraw},
    m_clearTileId{clearTileId},
    m_radius{radiusValue}
{}

void editor_tools::ScatterBrush::GenerateCandidateOffsets()
{
    using namespace sgc::math;
    using namespace sgc::tile;

    m_candidateOffsets.clear();

    auto cursorSize = m_tilesetSection.GetCursorSizeInTiles();

    if(m_radius < 0 || cursorSize.x <= 0 || cursorSize.y <= 0)
    {
        return;
    }

    for(int x=0; x < m_radius * cursorSize.x; x++){
        for(int y=0; y < (m_radius-x) * cursorSize.y; y++){
            m_candidateOffsets.push_back(sgc::tile::TilePosition2D{ x, y });
            m_candidateOffsets.push_back(sgc::tile::TilePosition2D{ x,-y });
            m_candidateOffsets.push_back(sgc::tile::TilePosition2D{-x, y });
            m_candidateOffsets.push_back(sgc::tile::TilePosition2D{-x,-y });
        }        
    }
}

std::mt19937 g_rng{std::random_device{}()};

void editor_tools::ScatterBrush::Execute(file::MapDocument& mapDocument, sgc::tile::TilePosition2D cursorPosition)
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

        GenerateCandidateOffsets();
    }
    
    if(m_candidateOffsets.empty()){
        return;
    }

    std::unordered_map<sgc::tile::TilePosition2D, command::TileChange> tileChanges;
    std::uniform_int_distribution<std::size_t> candidateIndex{0, m_candidateOffsets.size() - 1};
    sgc::tile::TilePosition2D candidate = cursorPosition + m_candidateOffsets[candidateIndex(g_rng)];

    auto positionOnTileset = m_tilesetSection.GetCursorPositionInTiles();
    auto cursorSize = m_tilesetSection.GetCursorSizeInTiles();
    auto tileSize = m_mapView.GetTileSize();

    auto endPosition = sgc::tile::TilePosition2D{
        cursorSize.x - 1,
        cursorSize.y - 1
    };   
    
    for(auto x = 0 ; x <= endPosition.x; ++x){
        for(auto y = 0; y <= endPosition.y; ++y)
        {
            auto currentPosition = sgc::tile::TilePosition2D{candidate.x + x, candidate.y + y};
            auto tileId = m_mapView.GetSgcTileset().GetTileId(positionOnTileset + sgc::tile::TilePosition2D{x,y});

            if(m_paintCommand->Contains(currentPosition))
            {
                tileChanges.clear();
                return;
            }

            auto checkTile = currentLayer->GetTileAt(currentPosition);

            if( m_allowChunkCreation || checkTile.has_value()) {

                if(!checkTile.has_value()){
                    auto chunkStorage = dynamic_cast<sgc::data::ChunkedTileStorage*>(currentLayer);
                    auto chunkCoord = chunkStorage->GetChunkCoordAt(currentPosition);
                    auto createChunkCommand = std::make_unique<command::CreateChunkCommand>(
                        chunkCoord,
                        mapDocument.GetLayerManager()->GetActiveLayerIndex(),
                        mapDocument,
                        m_clearTileId
                    );

                    createChunkCommand->Execute();
                    checkTile = currentLayer->GetTileAt(currentPosition);
                    m_bulkCommand->AddCommand(std::move(createChunkCommand));                    
                }

                command::TileChange change{
                    currentPosition,
                    checkTile,
                    tileId
                };
                
                tileChanges[currentPosition] = change;
            }
            
        }
    }

    m_paintCommand->ExecuteTileChange(tileChanges);
    return;
}

editor_tools::EditorToolUpdateRestriction editor_tools::ScatterBrush::GetUpdateRestriction()
{
    return EditorToolUpdateRestriction::Time;
}
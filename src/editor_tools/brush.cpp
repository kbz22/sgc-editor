#include "editor_tools/brush.hpp"

editor_tools::Brush::Brush(sgc::graphics::Rectangle& selectionRectangleOnTileset) :
    m_selectionRectangleOnTileset{selectionRectangleOnTileset}
{}

editor_tools::PaintMode editor_tools::Brush::GetPaintMode() const
{
    return m_paintMode;
}

void editor_tools::Brush::SetPaintMode(PaintMode paintMode)
{
    m_paintMode = paintMode;
}

void editor_tools::Brush::SetCheckTileBeforePainting(bool check)
{
    m_checkTileBeforePainting = check;
}

void editor_tools::Brush::PaintExecuteChange(
    file::MapDocument& mapDocument,
    sgc::graphics::Tileset& tileset,
    sgc::tile::TilePosition2D tilePosition,
    sgc::tile::TilePosition2D cursorPositionOnTileset,
    sgc::tile::TileSize2D tileSize
)
{
    if(m_paintCommand == nullptr) {
        auto layerManager = mapDocument.GetLayerManager();
        m_paintCommand = std::make_unique<command::PaintCommand>(
            &mapDocument,
            layerManager->GetActiveLayerIndex()
        );
    }

    if(m_selectionStart == nullptr) {
        m_selectionStart = std::make_unique<sgc::tile::TilePosition2D>(tilePosition);
    }

    auto currentLayer = mapDocument.GetCurrentLayerStorage();

    if(currentLayer == nullptr) {
        return;
    }    

    auto absmod = [](sgc::math::ival value, sgc::math::ival mod) -> sgc::math::ival {
        return ((value % mod) + mod) % mod;
    };

    for(auto x = 0; x < tileSize.x; ++x){
        for(auto y = 0; y < tileSize.y; ++y)
        {
            auto tileMapX = tilePosition.x + x;
            auto tileMapY = tilePosition.y + y;

            auto deltaX = absmod(
                tileMapX - m_selectionStart->x,
                tileSize.x
            );

            auto deltaY = absmod(
                tileMapY - m_selectionStart->y,
                tileSize.y
            );

            auto tileId = tileset.ToTileId(
                cursorPositionOnTileset.x + deltaX,
                cursorPositionOnTileset.y + deltaY
            );

            if( !m_checkTileBeforePainting || currentLayer->GetTileAt({ tileMapX, tileMapY }).has_value()) {

                command::TileChange change{
                    { tileMapX, tileMapY },
                    currentLayer->GetTileAt({ tileMapX, tileMapY }),
                    tileId
                };
                
                m_paintCommand->ExecuteTileChange(change);

            } 
        }
    }
    
}

void editor_tools::Brush::PaintCommitChanges(file::MapDocument& mapDocument)
{
    auto commandManager = mapDocument.GetCommandManager();
    commandManager->Commit(std::move(m_paintCommand));

    m_paintCommand.reset();
    m_selectionStart.reset();
}

#include "sgc_view/tileset_view.hpp"
#include "program/program.hpp"
#include <sgc/data/statictilestorage.hpp>
#include <windows.h>

sgc_view::TilesetView::TilesetView(HWND hwnd, bool &symbolMode, program::TileSymbolManager* symbolManager) :
    SgcView{hwnd},
    m_symbolMode{symbolMode},
    m_symbolManager{symbolManager},
    m_cursorTile{nullptr},
    m_symbolTile{std::make_unique<sgc::graphics::Rectangle>(0,0,0,0)},
    m_symbolCursor{std::make_unique<sgc::graphics::Rectangle>(0,0,0,0)}
{}

void sgc_view::TilesetView::SetCursorTile(sgc::graphics::PixelPosition2D position, sgc::graphics::PixelSize2D size)
{
    if(m_cursorTile == nullptr) {
        m_cursorTile = std::make_unique<graphics::Rectangle>(0, 0, 0, 0);
    }

    m_cursorTile->SetPosition({
        position.x,
        position.y
    });

    m_cursorTile->SetSize({
        size.x,
        size.y
    });

    m_cursorTile->SetColor(m_cursorColor);
}

void sgc_view::TilesetView::ResetCursorTile()
{
    m_cursorTile.reset();
}

void sgc_view::TilesetView::Render()
{
    SgcView::Clear();
    SgcView::DrawAll();

    if(m_symbolMode && m_symbolManager != nullptr && !m_symbolManager->IsEmpty())
    {
        auto group = m_symbolManager->PeekSymbolGroup();
        auto tileSize = GetTileSize();
        auto symbolSize = group->GetSize();
        auto positions = group->GetPositions();
        auto currentSymbol = group->PeekSymbol(m_sgcTileset).value();
        
        m_symbolTile->SetSize({
            symbolSize.x * tileSize.x,
            symbolSize.y * tileSize.y
        });

        for(auto &symbolPosition : positions)
        {
            if(symbolPosition == currentSymbol)
            {
                m_symbolCursor->SetSize({
                    symbolSize.x * tileSize.x,
                    symbolSize.y * tileSize.y
                });
                m_symbolCursor->SetPosition({
                    symbolPosition.x * tileSize.x,
                    symbolPosition.y * tileSize.y
                });
                m_symbolCursor->Draw(m_renderContext);
            }
            else
            {
                m_symbolTile->SetPosition({
                    symbolPosition.x * tileSize.x,
                    symbolPosition.y * tileSize.y
                });
                m_symbolTile->Draw(m_renderContext);
            }            
        }
    }
    else if(m_cursorTile != nullptr) 
    {
        m_cursorTile->Draw(m_renderContext);
    }

    sdl::Render(m_renderContext);
}

void sgc_view::TilesetView::SetTileset(sgc::data::AssetId tilesetId)
{
    SgcView::SetTileset(tilesetId);

    if(!m_sgcTileset.IsSet()) {
        return;
    }

    auto tileset = m_sgcTileset.GetTileset();
    const math::vec2 gridSize = tileset->GetSizeInTiles();
    auto tileStorage = std::make_shared<data::StaticTileStorage>(tile::TileSize2D{gridSize.x, gridSize.y});

    for (sgc::tile::TileId i = 0; i < gridSize.y * gridSize.x; ++i) {  
        tileStorage->SetTileAt({
            static_cast<sgc::math::ival>(i % gridSize.x),
            static_cast<sgc::math::ival>(i / gridSize.x)
        }, 
            i
    );
    }

    m_layer = std::make_shared<graphics::TiledLayer>(
        tileset,
        tileStorage
    );

    auto tiledImage = std::make_shared<graphics::TiledImage>(m_layer);    
    tiledImage->SetPositionPixels(sgc::graphics::PixelPosition2D{
        0,
        m_sgcTileset.GetVerticalOffsetInPixels()
    });
    m_drawableImage = tiledImage;
}

void sgc_view::TilesetView::ResetTileset()
{
    m_sgcTileset.Reset();
    m_layer.reset();
    m_drawableImage.reset();

    ResetCursorTile();

    Render();
}

void sgc_view::TilesetView::Refresh(program::ProgramContext& programContext)
{
    sgc_view::SgcView::Refresh(programContext);
    auto selectedDocument = programContext.GetManager<file::FileManager>()->GetActiveDocument();

    if(selectedDocument == nullptr && m_sgcTileset.IsSet()) {
        ResetTileset();
        return;
    }

    if(selectedDocument != nullptr && !m_sgcTileset.IsSet()) {
        auto tilesetId = selectedDocument->GetTilesetAssetId();
        SetTileset(tilesetId);
    }

    if(!m_sgcTileset.IsSet()) {
        return;
    }

    auto tileSize = GetTileSize();    

    if(m_cursorTile == nullptr) {
        SetCursorTile({
            0,
            0
        }, {
            static_cast<sgc::math::ival>(tileSize.x),
            static_cast<sgc::math::ival>(tileSize.y)
        });
    }

    m_symbolManager = selectedDocument->GetSymbolManager();
}

void sgc_view::TilesetView::SetCursorColor(sgc::graphics::color color)
{
    m_cursorColor = color;
}

void sgc_view::TilesetView::SetSymbolColor(sgc::graphics::color color)
{
    m_symbolTile->SetColor(color);
}

void sgc_view::TilesetView::SetSymbolCursorColor(sgc::graphics::color color)
{
    m_symbolCursor->SetColor(color);
}

void sgc_view::TilesetView::SetCursorPositionInPixels(sgc::graphics::PixelPosition2D position)
{
    if(m_cursorTile == nullptr) {
        return;
    }

    m_cursorTile->SetPosition({
        position.x,
        position.y
    });
}

void sgc_view::TilesetView::SetCursorSizeInPixels(sgc::graphics::PixelSize2D size)
{
    if(m_cursorTile == nullptr) {
        return;
    }

    m_cursorTile->SetSize({
        size.x,
        size.y
    });
}

sgc::graphics::PixelPosition2D sgc_view::TilesetView::GetCursorPositionInPixels() const
{
    if(m_cursorTile == nullptr) {
        return { 0, 0 };
    }

    return m_cursorTile->GetPosition();
}

sgc::graphics::PixelSize2D sgc_view::TilesetView::GetCursorSizeInPixels() const
{
    if(m_cursorTile == nullptr) {
        return { 0, 0 };
    }

    return m_cursorTile->GetSize();
}
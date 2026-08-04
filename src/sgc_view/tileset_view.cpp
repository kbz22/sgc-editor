#include "sgc_view/tileset_view.hpp"

#include <sgc/data/statictilestorage.hpp>

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <algorithm>
#include <stdexcept>

#include "debug.hpp"
#include "program/program.hpp"

sgc_view::TilesetView::TilesetView(HWND hwnd) :
    SgcView(hwnd),
    m_cursorTile{nullptr}
{}

sgc_view::TilesetView::~TilesetView() {
    // nothing to do
}

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

    if(m_cursorTile != nullptr) {
        m_cursorTile->Draw(m_renderContext);
    }

    sdl::Render(m_renderContext);
}

void sgc_view::TilesetView::SetTileset(sgc::data::AssetId tilesetId)
{
    SgcView::SetTileset(tilesetId);

    if(m_tileset == nullptr) {
        return;
    }

    const math::vec2 gridSize = m_tileset->GetSizeInTiles();

    auto tileStorage = std::make_shared<data::StaticTileStorage>(math::vec2{gridSize.x, gridSize.y});

    for (sgc::tile::TileId i = 0; i < gridSize.y * gridSize.x; ++i) {  
        tileStorage->SetTileAt({
            static_cast<sgc::math::ival>(i % gridSize.x),
            static_cast<sgc::math::ival>(i / gridSize.x)
        }, 
            i
    );
    }

    m_layer = std::make_shared<graphics::TiledLayer>(
        m_tileset,
        tileStorage
    );

    m_drawableImage = std::make_shared<graphics::TiledImage>(m_layer);
}

void sgc_view::TilesetView::ResetTileset()
{
    m_tileset.reset();
    m_layer.reset();
    m_drawableImage.reset();

    ResetCursorTile();

    Render();
}

void sgc_view::TilesetView::Refresh(program::ProgramContext& programContext)
{
    sgc_view::SgcView::Refresh(programContext);
    auto selectedDocument = programContext.fileManager->GetActiveDocument();

    if(selectedDocument == nullptr && m_tileset != nullptr) {
        ResetTileset();
        return;
    }

    if(selectedDocument != nullptr && m_tileset == nullptr) {
        auto tilesetId = selectedDocument->GetTilesetAssetId();
        SetTileset(tilesetId);
    }

    if(m_tileset == nullptr) {
        return;
    }

    auto tileSize = m_tileset->GetTileSize();

    if(m_cursorTile == nullptr) {
        SetCursorTile({
            0,
            0
        }, {
            static_cast<sgc::math::ival>(tileSize.x),
            static_cast<sgc::math::ival>(tileSize.y)
        });
    }
}

void sgc_view::TilesetView::SetCursorColor(sgc::graphics::color color)
{
    m_cursorColor = color;
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
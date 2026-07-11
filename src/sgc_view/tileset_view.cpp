#include "sgc_view/tileset_view.hpp"

#include <sgc/data/statictilestorage.hpp>

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <algorithm>
#include <stdexcept>

#include "debug.hpp"
#include "program/program.hpp"

sgc_view::TilesetView::TilesetView(HWND hwnd, std::filesystem::path tilesetPath, int tileWidth, int tileHeight) :
    SgcView(hwnd, tileWidth, tileHeight) 
{
    m_tileset = LoadTileset(tilesetPath);
    if(!m_tileset) {
        throw std::runtime_error("Failed to load tileset from path: " + tilesetPath.string());
    }
    
    const math::uvec2 gridSize = m_tileset->GetSizeInTiles();

    auto tileStorage = std::make_shared<data::StaticTileStorage>(math::uvec2{gridSize.x, gridSize.y});

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

sgc_view::TilesetView::~TilesetView() {
    // nothing to do
}

void sgc_view::TilesetView::Render()
{
    SgcView::Clear();
    SgcView::DrawAll();

    program::ProgramContext& programContext = program::GetProgramContext();

    if (programContext.selectionRectangleOnTileset != nullptr) {
        programContext.selectionRectangleOnTileset->Draw(m_renderContext);
    }

    sdl::Render(m_renderContext);
}
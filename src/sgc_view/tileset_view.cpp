#include "sgc_view/tileset_view.hpp"

#include <sgc/data/statictilestorage.hpp>

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <algorithm>

#include "debug.hpp"
#include "program/program.hpp"

sgc_view::TilesetView::TilesetView(HWND hwnd) : SgcView(hwnd) 
{   
}

sgc_view::TilesetView::~TilesetView() {
    // nothing to do
}

bool sgc_view::TilesetView::LoadTileset(const std::wstring& path, int tileWidth, int tileHeight)
{
    if(!SgcView::LoadTileset(path, tileWidth, tileHeight)) {
        return false;
    }

    m_highlightedTile = graphics::Rectangle({ 0, 0 }, { 32, 32 });

    m_highlightedTile.SetColor({ 0, 128, 255, 128 });
    
    const math::uvec2 gridSize = m_tileset->GetSizeInTiles();

    auto tileStorage = std::make_shared<data::StaticTileStorage>(math::uvec2{gridSize.x, gridSize.y});

    for (uint64_t i = 0; i < gridSize.y * gridSize.x; ++i) {  
            tileStorage->SetTileAt({ i % gridSize.x, i / gridSize.x }, i);
    }

    m_layer = std::make_shared<graphics::TiledLayer>(
        m_tileset,
        tileStorage
    );

    m_tiledImage = std::make_shared<graphics::TiledImage>(m_layer);

    Render();

    return true;
}

LRESULT sgc_view::TilesetView::HandleMessages([[maybe_unused]] HWND hwnd, [[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wparam, [[maybe_unused]] LPARAM lparam)
{
    using namespace program;
    ProgramContext& programContext = program::GetProgramContext();

    switch (msg)
    {
        case WM_LBUTTONDOWN:
        {
            int x = GET_X_LPARAM(lparam);
            int y = GET_Y_LPARAM(lparam);

            auto bounds = m_tileset->GetImageSize();

            if(x > bounds.x || y > bounds.y) {
                break;
            }

            m_highlightedTile.SetPosition({
                (x / m_tileWidth) * m_tileWidth,
                (y / m_tileHeight) * m_tileHeight
            });      

            Render();
            programContext.mapView->SetTile(x / m_tileWidth, y / m_tileHeight);
            programContext.mapView->Render();
            
            return 0;
        }

        case WM_DESTROY:
            m_sectionWindow = nullptr;
            break;
    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

void sgc_view::TilesetView::Render()
{
    SgcView::DrawAll();
    m_highlightedTile.Draw(m_renderContext);

    sdl::Render(m_renderContext);
}
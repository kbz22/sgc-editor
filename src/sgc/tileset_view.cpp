#include "sgc/tileset_view.hpp"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include "debug.hpp"

sgc::TilesetView::TilesetView(HWND hwnd) : SgcView(hwnd) 
{   
}

sgc::TilesetView::~TilesetView() {
    // nothing to do
}

bool sgc::TilesetView::LoadTileset(const std::wstring& path, int tileWidth, int tileHeight)
{
    if(!SgcView::LoadTileset(path, tileWidth, tileHeight)) {
        return false;
    }

    m_highlightedTile = image::Rectangle{
        0,
        0,
        static_cast<int>(m_tileWidth),
        static_cast<int>(m_tileHeight) 
    };

    m_highlightedTile.SetColor({ 255, 0, 0, 128 });
    m_highlightedTile.SetRenderer(m_renderer);

    const types::uvec2 imageSize = m_tileset->GetImageSize(); 

    m_gridWidth  = imageSize.x / m_tileWidth;
    m_gridHeight = imageSize.y / m_tileHeight;

    std::vector<types::uvec2> tilePositions;
    tilePositions.reserve(static_cast<size_t>(m_gridWidth) * static_cast<size_t>(m_gridHeight));

    for (types::unsignedint_t row = 0; row < m_gridHeight; ++row) {
        for (types::unsignedint_t col = 0; col < m_gridWidth; ++col) {
            tilePositions.push_back({ col, row });
        }
    }

    m_layer = std::make_unique<image::TiledStaticLayer>(
        m_tileset,
        std::move(tilePositions),
        types::uvec2{ m_gridWidth, m_gridHeight },
        types::vec2{ 0, 0 }
    );

    m_layer->SetRenderer(m_renderer);

    Render();

    return true;
}

LRESULT sgc::TilesetView::HandleMessages([[maybe_unused]] HWND hwnd, [[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wparam, [[maybe_unused]] LPARAM lparam)
{

    switch (msg)
    {
        case WM_LBUTTONDOWN:
        {
            int x = GET_X_LPARAM(lparam);
            int y = GET_Y_LPARAM(lparam);

            m_highlightedTile.SetPosition({x, y});
            m_highlightedTile.Draw();
            
            return 0;
        }

        case WM_DESTROY:
            m_sectionWindow = nullptr;
            break;
    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

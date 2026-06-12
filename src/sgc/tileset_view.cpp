#include "sgc/tileset_view.hpp"

sgc::TilesetView::TilesetView(HWND hwnd) : SgcView(hwnd) 
{}

sgc::TilesetView::~TilesetView() {
    // nothing to do
}

bool sgc::TilesetView::LoadTileset(const std::wstring& path, int tileWidth, int tileHeight)
{
    if(!SgcView::LoadTileset(path, tileWidth, tileHeight)) {
        return false;
    }

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

    Render();

    return true;
}
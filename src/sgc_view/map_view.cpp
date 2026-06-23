#include "sgc_view/map_view.hpp"
#include <windows.h>
#include <commctrl.h>

sgc_view::MapView::MapView(HWND hwnd) : SgcView(hwnd)
{
}

sgc_view::MapView::~MapView()
{    
}

void sgc_view::MapView::SetTile(int tileX, int tileY)
{
    if (m_tileStorage == nullptr) {
        return;
    }

    m_tileStorage->SetTileAt({ 0, 0 }, m_tileset->ToTileId(tileX, tileY));

    Render();    
}

LRESULT sgc_view::MapView::HandleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

bool sgc_view::MapView::LoadTileset(const std::wstring& path, int tileWidth, int tileHeight)
{
    if(!SgcView::LoadTileset(path, tileWidth, tileHeight)) {
        return false;
    }

    m_tileStorage = std::make_shared<data::StaticTileStorage>(math::uvec2{1, 1});

    auto layer = std::make_shared<graphics::TiledLayer>(
        m_tileset,
        m_tileStorage
    );

    m_tiledImage = std::make_shared<graphics::TiledImage>(layer);

    Render();

    return true;
}
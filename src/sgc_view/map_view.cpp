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
    m_tilePosition = math::vec2{ tileX, tileY };
    m_layer = std::make_unique<graphics::TiledStaticLayer>(
        m_tileset,
        std::vector<math::uvec2>{ { static_cast<math::u64>(tileX), static_cast<math::u64>(tileY) } },
        math::uvec2{ 1, 1 },
        math::vec2{ 0, 0 }
    );
}

LRESULT sgc_view::MapView::HandleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    return DefSubclassProc(hwnd, msg, wparam, lparam);
}
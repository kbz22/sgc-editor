#include "sgc_view/map_view.hpp"

sgc_view::MapView::MapView(HWND hwnd) : SgcView(hwnd)
{
}

sgc_view::MapView::~MapView()
{    
}

void sgc_view::MapView::SetTile(int tileX, int tileY)
{
    m_tilePosition = math::vec2{ tileX, tileY };
}

void sgc_view::MapView::Render()
{

}
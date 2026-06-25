#include "sgc_view/map_view.hpp"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

sgc_view::MapView::MapView(HWND hwnd) : SgcView(hwnd)
{
    m_tileStorage = std::make_shared<data::ChunkedTileStorage>();
    m_tileStorage->SetChunkAt({ 0, 0 }, 0);
}

sgc_view::MapView::~MapView()
{    
}

void sgc_view::MapView::SetTile(int tileX, int tileY)
{
    m_currentTileId = m_tileset->ToTileId(static_cast<math::u64>(tileX), static_cast<math::u64>(tileY));   
}

void sgc_view::MapView::SetTile(tile::TileId tileId)
{
    m_currentTileId = tileId;
}

LRESULT sgc_view::MapView::HandleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
        case WM_LBUTTONDOWN:
        {
            auto tileSize = m_tileset->GetTileSize();
            auto x = static_cast<sgc::math::u64>(GET_X_LPARAM(lparam) / tileSize.x);
            auto y = static_cast<sgc::math::u64>(GET_Y_LPARAM(lparam) / tileSize.y);

            if(m_tileStorage->GetTileAt({ x, y }).has_value()) {
                m_tileStorage->SetTileAt({ x, y }, m_currentTileId);
            }

            Render();
            
            return 0;
        }

        case WM_DESTROY:
            m_sectionWindow = nullptr;
            break;
    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

bool sgc_view::MapView::LoadTileset(const std::wstring& path, int tileWidth, int tileHeight)
{
    if(!SgcView::LoadTileset(path, tileWidth, tileHeight)) {
        return false;
    }    

    auto layer = std::make_shared<graphics::TiledLayer>(
        m_tileset,
        m_tileStorage
    );

    m_tiledImage = std::make_shared<graphics::TiledImage>(layer);

    Render();

    return true;
}

void sgc_view::MapView::AddChunk(sgc::data::ChunkCoord chunkCoord)
{
    if (m_tileStorage == nullptr) {
        return;
    }

    m_tileStorage->SetChunkAt(chunkCoord, 0);
}

void sgc_view::MapView::RemoveChunk(sgc::data::ChunkCoord chunkCoord)
{
    if (m_tileStorage == nullptr) {
        return;
    }

    m_tileStorage->RemoveChunkAt(chunkCoord);
}
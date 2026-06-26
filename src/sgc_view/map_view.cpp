#include "sgc_view/map_view.hpp"
#include "program/program.hpp"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

sgc_view::MapView::MapView(HWND hwnd, int tileWidth, int tileHeight) :
    SgcView(hwnd, tileWidth, tileHeight)
{
    m_tileStorage = std::make_shared<data::ChunkedTileStorage>();
    m_tileStorage->SetChunkAt({ 0, 0 }, 0);
}

sgc_view::MapView::~MapView()
{    
}

LRESULT sgc_view::MapView::HandleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{ 
    program::ProgramContext& programContext = program::GetProgramContext();

    switch (msg)
    {
        case WM_LBUTTONDOWN:
        {
            auto tileSize = m_tileset->GetTileSize();
            auto x = static_cast<sgc::math::u64>(GET_X_LPARAM(lparam) / tileSize.x);
            auto y = static_cast<sgc::math::u64>(GET_Y_LPARAM(lparam) / tileSize.y);

            auto currentTilePosition = programContext.selectionRectangle->GetPosition();
            //auto currentTileId = m_tileset->ToTileId(static_cast<sgc::math::u64>(currentTilePosition.x / tileSize.x), static_cast<sgc::math::u64>(currentTilePosition.y / tileSize.y));

            auto tileWidth = static_cast<sgc::math::u64>(programContext.selectionRectangle->GetSize().x / tileSize.x);
            auto tileHeight = static_cast<sgc::math::u64>(programContext.selectionRectangle->GetSize().y / tileSize.y);

            for(auto _x = 0; _x < tileWidth; ++_x) {
                for(auto _y = 0; _y < tileHeight; ++_y) {
                    auto tileX = x + _x;
                    auto tileY = y + _y;
                    auto currentTileId = m_tileset->ToTileId(static_cast<sgc::math::u64>(currentTilePosition.x / tileSize.x) + _x, static_cast<sgc::math::u64>(currentTilePosition.y / tileSize.y) + _y);

                    if(m_tileStorage->GetTileAt({ tileX, tileY }).has_value()) {
                        m_tileStorage->SetTileAt({ tileX, tileY }, currentTileId);
                    }                       
                }
            }

            Render();
            
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            auto x = GET_X_LPARAM(lparam);
            auto y = GET_Y_LPARAM(lparam);

            auto x_tile = static_cast<sgc::math::i64>(x - (x % m_tileWidth));
            auto y_tile = static_cast<sgc::math::i64>(y - (y % m_tileHeight));

            auto position = m_cursorTile.GetPosition();
            if (position.x != x_tile || position.y != y_tile) {
                
                auto selection = programContext.selectionRectangle->GetSize();

                m_cursorTile.SetSize({
                    static_cast<sgc::math::u64>(selection.x),
                    static_cast<sgc::math::u64>(selection.y)
                });

                m_cursorTile.SetPosition({
                    x_tile,
                    y_tile
                });

                Render();
            }

            return 0;
        }

        case WM_DESTROY:
            m_sectionWindow = nullptr;
            break;
    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

bool sgc_view::MapView::LoadTileset(const std::wstring& path)
{
    if(!SgcView::LoadTileset(path)) {
        return false;
    }

    m_cursorTile = graphics::Rectangle({
        0,
        0
    }, {
        static_cast<sgc::math::u64>(m_tileWidth),
        static_cast<sgc::math::u64>(m_tileHeight)
    });

    m_cursorTile.SetColor({ 255, 255, 255, 64 });

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

void sgc_view::MapView::Render()
{
    SgcView::Clear();
    SgcView::DrawAll();

    m_cursorTile.Draw(m_renderContext);

    sdl::Render(m_renderContext);
}
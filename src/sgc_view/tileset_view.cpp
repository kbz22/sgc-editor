#include "sgc_view/tileset_view.hpp"

#include <sgc/data/statictilestorage.hpp>

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <algorithm>

#include "debug.hpp"
#include "program/program.hpp"

sgc_view::TilesetView::TilesetView(HWND hwnd, int tileWidth, int tileHeight) :
    SgcView(hwnd, tileWidth, tileHeight) 
{   
}

sgc_view::TilesetView::~TilesetView() {
    // nothing to do
}

bool sgc_view::TilesetView::LoadTileset(const std::wstring& path)
{
    if(!SgcView::LoadTileset(path)) {
        return false;
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

            sgc::math::uvec2 tileSize = m_tileset->GetTileSize();
            sgc::math::vec2 tilePosition = {
                static_cast<sgc::math::ival>(x / tileSize.x),
                static_cast<sgc::math::ival>(y / tileSize.y)
            };

            m_selectionTileStart = tilePosition;
            m_selectionTileSize = { 1, 1 };
            
            programContext.selectionRectangleOnTileset->SetPosition(
                {tilePosition.x * static_cast<sgc::math::ival>(tileSize.x),
                tilePosition.y * static_cast<sgc::math::ival>(tileSize.y)}
            );
            programContext.selectionRectangleOnTileset->SetSize(
                {m_selectionTileSize.x * static_cast<sgc::math::ival>(tileSize.x), m_selectionTileSize.y * static_cast<sgc::math::ival>(tileSize.y)}
            );
            
            m_selectionActive = true;
            SetCapture(hwnd);

            Render();           
            
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            if (!m_selectionActive) {
                break;
            }

            int x = GET_X_LPARAM(lparam);
            int y = GET_Y_LPARAM(lparam);

            sgc::math::uvec2 tileSize = m_tileset->GetTileSize();

            sgc::math::uvec2 currentTile = {
                x / tileSize.x,
                y / tileSize.y
            };

            sgc::math::uvec2 minTile = {
                std::min(static_cast<sgc::math::uval>(m_selectionTileStart.x), currentTile.x),
                std::min(static_cast<sgc::math::uval>(m_selectionTileStart.y), currentTile.y)
            };

            sgc::math::uvec2 maxTile = {
                std::max(static_cast<sgc::math::uval>(m_selectionTileStart.x), currentTile.x),
                std::max(static_cast<sgc::math::uval>(m_selectionTileStart.y), currentTile.y)
            };

            m_selectionTileSize = {
                maxTile.x - minTile.x + 1,
                maxTile.y - minTile.y + 1
            };

            programContext.selectionRectangleOnTileset->SetPosition({
                static_cast<sgc::math::ival>(minTile.x * tileSize.x),
                static_cast<sgc::math::ival>(minTile.y * tileSize.y)
            });

            programContext.selectionRectangleOnTileset->SetSize({
                m_selectionTileSize.x * tileSize.x,
                m_selectionTileSize.y * tileSize.y
            });

            Render();            
            
            return 0;
        } 
        
        case WM_LBUTTONUP:
        {
            ReleaseCapture();

            if (m_selectionActive)
            {
                m_selectionActive = false;
            }

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
    SgcView::Clear();
    SgcView::DrawAll();

    program::ProgramContext& programContext = program::GetProgramContext();

    if (programContext.selectionRectangleOnTileset != nullptr) {
        programContext.selectionRectangleOnTileset->Draw(m_renderContext);
    }

    sdl::Render(m_renderContext);
}
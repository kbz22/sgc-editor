#include "sections/tileset_section.hpp"
#include "program/program.hpp"
#include <windowsx.h>
#include <commctrl.h>
#include <algorithm>

sections::TilesetSection::TilesetSection(win32_program::MainWindowContext& context) :
    Section{L"TilesetView", win32_program::ControlId::TilesetView, context},
    m_tilesetView{nullptr}
{}

void sections::TilesetSection::Update()
{
    if (m_tilesetView != nullptr) {
        m_tilesetView->Render();
    }   
}

void sections::TilesetSection::LoadTileset(const std::filesystem::path& path, int tileWidth, int tileHeight)
{   
    if(m_tilesetView != nullptr) {
        m_tilesetView.reset();
    }
    
    m_tilesetView = std::make_unique<sgc_view::TilesetView>(GetHwnd(), path, tileWidth, tileHeight);
    AttachView(*m_tilesetView);
}

void sections::TilesetSection::ClearTileset()
{
    m_tilesetView.reset();
}

void sections::TilesetSection::HandleSectionResize()
{
    if (m_tilesetView != nullptr) {
        RECT rect;
        GetClientRect(GetHwnd(), &rect);
        m_tilesetView->SetScreenSize(rect.right - rect.left, rect.bottom - rect.top);        
    }
    Update();    
}

LRESULT sections::TilesetSection::HandleMessages([[maybe_unused]] HWND hwnd, [[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wparam, [[maybe_unused]] LPARAM lparam)
{
    using namespace program;
    ProgramContext& programContext = program::GetProgramContext();

    switch (msg)
    {        
        case WM_LBUTTONDOWN:
        {
            int x = GET_X_LPARAM(lparam);
            int y = GET_Y_LPARAM(lparam);

            auto tileset = m_tilesetView->GetTileset();
            auto bounds = tileset->GetImageSize();

            if(x > bounds.x || y > bounds.y) {
                break;
            }

            sgc::math::uvec2 tileSize = tileset->GetTileSize();
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

            Update();
            
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            if (!m_selectionActive) {
                break;
            }

            int x = GET_X_LPARAM(lparam);
            int y = GET_Y_LPARAM(lparam);

            auto tileset = m_tilesetView->GetTileset();
            sgc::math::uvec2 tileSize = tileset->GetTileSize();

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

            Update();    
            
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
    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}
#include "sections/tileset_section.hpp"
#include "program/program.hpp"
#include <windowsx.h>
#include <commctrl.h>
#include <algorithm>

sections::TilesetSection::TilesetSection(program::ProgramContext& programContext) :
    Section{L"TilesetView", win32_program::ControlId::TilesetView, *programContext.mainWindowContext},
    m_tilesetView{std::make_unique<sgc_view::TilesetView>(GetHwnd())}
{}

void sections::TilesetSection::Update()
{
    Redraw();
    if (m_tilesetView != nullptr) {
        m_tilesetView->Render();
    }    
}

/* void sections::TilesetSection::LoadTileset(const std::filesystem::path& path, int tileWidth, int tileHeight, program::ProgramContext& programContext)
{
    if(m_tilesetView != nullptr) {
        m_tilesetView.reset();
    }

    sgc::data::AssetId id = sgc::data::HashAsset(path.string());
    m_tilesetView = std::make_unique<sgc_view::TilesetView>(GetHwnd());

    auto &renderContext = m_tilesetView->GetRenderContext();

    if(renderContext.renderer == nullptr) {
        renderContext.renderer = sgc::sdl::CreateRenderer(m_tilesetView->GetSdlWindow());
    }
    
    if(renderContext.renderer == nullptr) {
        throw std::runtime_error("Failed to create SDL_Renderer for TilesetView");
    }      

    auto tileset = programContext.assetManager->LoadTileset(
        id,
        path,
        tileWidth,
        tileHeight,
        &renderContext
    );

    m_tilesetView->SetTileset(tileset);
    m_tilesetView->SetRenderer(renderContext);
    
    AttachView(*m_tilesetView);
} */

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

void sections::TilesetSection::Refresh(program::ProgramContext& programContext)
{
    if (m_tilesetView != nullptr) {
        m_tilesetView->Refresh(programContext);
    }
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

            if(x < 0 || y < 0 || x >= bounds.x || y >= bounds.y) {
                break;
            }

            sgc::math::vec2 tileSize = tileset->GetTileSize();
            sgc::math::vec2 tilePosition = {
                static_cast<sgc::math::ival>(x / tileSize.x),
                static_cast<sgc::math::ival>(y / tileSize.y)
            };

            m_selectionTileStart = tilePosition;
            m_selectionTileSize = { 1, 1 };
            
            programContext.selectionRectangleOnTileset->SetPosition(
                {tilePosition.x * tileSize.x,
                tilePosition.y * tileSize.y}
            );
            programContext.selectionRectangleOnTileset->SetSize({
                m_selectionTileSize.x * tileSize.x,
                m_selectionTileSize.y * tileSize.y
            }
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

            x = std::max(x, 0); // in case a negative slips in
            y = std::max(y, 0);

            auto tileset = m_tilesetView->GetTileset();
            auto tileSize = tileset->GetTileSize();
            auto imageSize = tileset->GetImageSize();

            sgc::math::vec2 tileCount = {
                imageSize.x / tileSize.x,
                imageSize.y / tileSize.y
            };

            sgc::math::vec2 currentTile = {
                x / tileSize.x,
                y / tileSize.y
            };

            currentTile.x = std::min(currentTile.x, tileCount.x - 1);
            currentTile.y = std::min(currentTile.y, tileCount.y - 1);

            sgc::math::vec2 minTile = {
                std::min(m_selectionTileStart.x, currentTile.x),
                std::min(m_selectionTileStart.y, currentTile.y)
            };

            sgc::math::vec2 maxTile = {
                std::max(m_selectionTileStart.x, currentTile.x),
                std::max(m_selectionTileStart.y, currentTile.y)
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

        case WM_PAINT:
        {
            PAINTSTRUCT ps;

            HDC hdc = BeginPaint(hwnd, &ps);

            RECT clientRect;
            GetClientRect(hwnd, &clientRect);            

            FillRect(
                hdc,
                &ps.rcPaint,
                (HBRUSH)(COLOR_WINDOW + 1)
            );

            EndPaint(hwnd, &ps);
            return 0;
        }
    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}
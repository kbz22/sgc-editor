#include "sections/tileset_section.hpp"
#include "program/program.hpp"
#include <windowsx.h>
#include <commctrl.h>
#include <algorithm>

sections::TilesetSection::TilesetSection(program::ProgramContext& programContext) :
    Section{L"TilesetView", win32_program::ControlId::TilesetView, programContext.GetMainWindowHandle(), programContext.GetHInstance()},
    m_tilesetView{std::make_unique<sgc_view::TilesetView>(GetHwnd())}
{
    AttachView(*m_tilesetView);    
}

void sections::TilesetSection::UpdateStatusBar(sgc::math::vec2 position, std::optional<sgc::tile::TileId> tileId, sgc::math::vec2 size)
{
    auto &programContext = program::GetProgramContext();
    auto fileManager = programContext.GetManager<file::FileManager>(); 
    auto mapDocument = fileManager->GetActiveDocument();

    if(mapDocument != nullptr && mapDocument->IsEditable())
    {
        auto mapSection = programContext.GetSection<sections::MapSection>();        
        if(mapSection->GetPaintMode() != editor_tools::PaintMode::Select)
        {        
            auto statusSection = programContext.GetSection<sections::StatusSection>();
            auto &sgcTileset = m_tilesetView->m_sgcTileset;

            statusSection->SetStatusCursorPosition(position);
            statusSection->SetStatusSelectionSize(size);

            if(tileId.has_value() && sgcTileset.IsSpecial(tileId.value())){
                statusSection->SetStatusTileId(sgcTileset.GetSpecialTileName(tileId.value()));
            }
            else{
                statusSection->SetStatusTileId(tileId);
            }            
        }
    }
}

void sections::TilesetSection::Update()
{
    Redraw();
    if (m_tilesetView != nullptr) {
        m_tilesetView->Render();
    }    
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
    auto mapDocument = programContext.GetManager<file::FileManager>()->GetActiveDocument();

    if(mapDocument == nullptr || !mapDocument->IsEditable())
    {
        return DefSubclassProc(hwnd, msg, wparam, lparam);
    }

    switch (msg)
    {        
        case WM_LBUTTONDOWN:
        {
            int x = GET_X_LPARAM(lparam);
            int y = GET_Y_LPARAM(lparam);

            auto tileset = m_tilesetView->m_sgcTileset.GetTileset();
            sgc::math::vec2 tileSize = tileset->GetTileSize();
            sgc::math::vec2 tilePosition = {
                static_cast<sgc::math::ival>(x / tileSize.x),
                static_cast<sgc::math::ival>(y / tileSize.y)
            };

            if(!m_tilesetView->m_sgcTileset.IsValid(tilePosition)){
                break;
            }

            m_selectionTileStart = tilePosition;
            m_selectionTileSize = { 1, 1 };

            m_tilesetView->SetCursorPositionInPixels({
                static_cast<sgc::math::ival>(tilePosition.x * tileSize.x),
                static_cast<sgc::math::ival>(tilePosition.y * tileSize.y)
            });

            m_tilesetView->SetCursorSizeInPixels({
                static_cast<sgc::math::ival>(tileSize.x),
                static_cast<sgc::math::ival>(tileSize.y)
            });
            
            m_selectionActive = true;
            SetCapture(hwnd);

            Update();
            
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            int x = GET_X_LPARAM(lparam);
            int y = GET_Y_LPARAM(lparam);

            x = std::max(x, 0); // in case a negative slips in
            y = std::max(y, 0);

            auto tileset = m_tilesetView->m_sgcTileset.GetTileset();
            auto tileSize = tileset->GetTileSize();
            
            sgc::math::vec2 currentTile = {
                x / tileSize.x,
                y / tileSize.y
            };

            auto imageSize = tileset->GetImageSize();

            sgc::math::vec2 tileCount = {
                imageSize.x / tileSize.x,
                imageSize.y / tileSize.y
            };            

            /* if(currentTile.x >= tileCount.x || currentTile.y >= tileCount.y)
            {
                UpdateStatusBar(currentTile, std::nullopt, m_selectionTileSize);
            }
            else
            {
                auto tileId = tileset->ToTileId(currentTile.x, currentTile.y);
                UpdateStatusBar(currentTile, tileId, m_selectionTileSize);
            } */

            UpdateStatusBar(
                currentTile,
                m_tilesetView->m_sgcTileset.GetTileId(sgc::tile::TilePosition2D{currentTile.x, currentTile.y}),
                m_selectionTileSize
            );

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

            if (!m_selectionActive) {
                break;
            }            

            m_selectionTileSize = {
                maxTile.x - minTile.x + 1,
                maxTile.y - minTile.y + 1
            };

            m_tilesetView->SetCursorPositionInPixels({
                static_cast<sgc::math::ival>(minTile.x * tileSize.x),
                static_cast<sgc::math::ival>(minTile.y * tileSize.y)
            });

            m_tilesetView->SetCursorSizeInPixels({
                static_cast<sgc::math::ival>(m_selectionTileSize.x * tileSize.x),
                static_cast<sgc::math::ival>(m_selectionTileSize.y * tileSize.y)
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

void sections::TilesetSection::SetCursorPositionInPixels(sgc::graphics::PixelPosition2D position)
{
    if(m_tilesetView != nullptr) {
        m_tilesetView->SetCursorPositionInPixels(position);
    }
}

void sections::TilesetSection::SetCursorSizeInPixels(sgc::graphics::PixelSize2D size)
{
    if(m_tilesetView != nullptr) {
        m_tilesetView->SetCursorSizeInPixels(size);
    }
}

std::optional<sgc::graphics::PixelPosition2D> sections::TilesetSection::GetCursorPositionInPixels() const
{
    if(m_tilesetView != nullptr) {
        return m_tilesetView->GetCursorPositionInPixels();
    }

    return std::nullopt;
}

std::optional<sgc::graphics::PixelSize2D> sections::TilesetSection::GetCursorSizeInPixels() const
{
    if(m_tilesetView != nullptr) {
        return m_tilesetView->GetCursorSizeInPixels();
    }

    return std::nullopt;
}

sgc::tile::TileId sections::TilesetSection::GetClearTileId() const
{
    /* if(m_tilesetView != nullptr) {
        // return m_tilesetView->m_tileset->TileIdCount();
        return m_tilesetView->m_sgcTileset.GetTileset().TileIdCount();
    }

    return 0; */
    return m_tilesetView->GetClearTileId();
}
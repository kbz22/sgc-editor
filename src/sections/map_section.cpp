#include "sections/map_section.hpp"
#include "program/program.hpp"
#include "command/paint_command.hpp"
#include "command/command_manager.hpp"

#include <sgc/coordinates/screenworld.hpp>
#include <sgc/sdl/sdl_win32.hpp>
#include <fstream>
#include <filesystem>
#include <windowsx.h>
#include <commctrl.h>
#include <cmath>

sections::MapSection::MapSection(program::ProgramContext& programContext) :
    Section{L"MapView", win32_program::ControlId::MapView, *programContext.mainWindowContext},
    m_mapView{std::make_unique<sgc_view::MapView>(GetHwnd())},
    m_brush{*programContext.selectionRectangleOnTileset}
{
    AttachView(*m_mapView);
}

void sections::MapSection::Update()
{    
    Redraw();
    if (m_mapView != nullptr) {        
        m_mapView->Render();
    }    
}

void sections::MapSection::Refresh(program::ProgramContext& programContext)
{    
    if (m_mapView != nullptr) {
        m_mapView->Refresh(programContext);
    }
}

void sections::MapSection::HandleSectionResize()
{
    if (m_mapView != nullptr) {
        RECT rect;
        GetClientRect(GetHwnd(), &rect);
        m_mapView->SetScreenSize(rect.right - rect.left, rect.bottom - rect.top);        
    }
    Update();
}

LRESULT sections::MapSection::HandleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{ 
    program::ProgramContext& programContext = program::GetProgramContext();
    auto mapDocument = programContext.fileManager->GetSelectedDocument();
    auto &tilesetSection = programContext.tilesetSection;

    if(mapDocument == nullptr || !mapDocument->IsEditable())
    {
        return DefSubclassProc(hwnd, msg, wparam, lparam);
    }

    auto SetCaptureHelper = [this](HWND hwnd) {
        if (!m_isCaptured) {
            SetCapture(hwnd);
            m_isCaptured = true;
        }
    };

    auto ReleaseCaptureHelper = [this]() {
        if (m_isCaptured) {
            ReleaseCapture();
            m_isCaptured = false;
        }
    };

    auto IsMouseCaptured = [this]() -> bool {
        return m_isCaptured;
    };

    switch (msg)
    {
        case WM_LBUTTONDOWN:
        {
            if(IsMouseCaptured()) {
                return 0;
            }

            auto tileset = m_mapView->GetTileset();
            auto tileSize = tileset->GetTileSize();

            auto mousePos = m_mapView->PixelsToTiles(
            sgc::math::vec2{ 
                GET_X_LPARAM(lparam),
                GET_Y_LPARAM(lparam)
            });
            
            auto currentTilePosition = tilesetSection->GetCursorPositionInPixels().value_or(sgc::graphics::PixelPosition2D{0, 0});
            auto tileWidth = static_cast<sgc::math::ival>(tilesetSection->GetCursorSizeInPixels().value_or(sgc::graphics::PixelSize2D{tileSize.x, tileSize.y}).x / tileSize.x);
            auto tileHeight = static_cast<sgc::math::ival>(tilesetSection->GetCursorSizeInPixels().value_or(sgc::graphics::PixelSize2D{tileSize.x, tileSize.y}).y / tileSize.y);

            auto cursorPositionOnMap = m_mapView->GetCursorPositionInTiles();

            m_brush.PaintExecuteChange(
                *mapDocument,
                *tileset,
                cursorPositionOnMap,
                sgc::tile::TilePosition2D{
                    static_cast<sgc::math::ival>(currentTilePosition.x / tileSize.x),
                    static_cast<sgc::math::ival>(currentTilePosition.y / tileSize.y)
                },
                sgc::tile::TileSize2D{tileWidth, tileHeight}
            );

            m_isPainting = true;
            SetCaptureHelper(hwnd);        

            Update();            
            
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            if(m_isPanning) {
                auto x = static_cast<sgc::math::ival>(GET_X_LPARAM(lparam));
                auto y = static_cast<sgc::math::ival>(GET_Y_LPARAM(lparam));

                auto deltaX = x - m_lastMousePosPan.x;
                auto deltaY = y - m_lastMousePosPan.y;

                m_mapView->ChangeCameraPositionSingles(
                    static_cast<float>(deltaX),
                    static_cast<float>(deltaY)
                );

                m_lastMousePosPan.x = x;
                m_lastMousePosPan.y = y;

                Update();
                return 0;
            }

            bool shouldUpdate = false;

            auto cameraPosition = m_mapView->GetCameraPositionSingles();

            auto screenPosition = sgc::math::fvec2{
                static_cast<float>(GET_X_LPARAM(lparam)),
                static_cast<float>(GET_Y_LPARAM(lparam))
            };

            auto view = m_mapView->GetView();
            auto worldPosition = sgc::coordinates::ScreenToWorld(screenPosition, view);

            auto tileSize = m_mapView->GetTileSize();
            auto x_tile = static_cast<sgc::math::ival>(
                std::floor(worldPosition.x / tileSize.x) * tileSize.x
            );

            auto y_tile = static_cast<sgc::math::ival>(
                std::floor(worldPosition.y / tileSize.y) * tileSize.y
            );

            auto position = m_mapView->GetCursorPositionInTiles();
            if (position.x != x_tile || position.y != y_tile) {                
                
                auto selection = tilesetSection->GetCursorSizeInPixels().value_or(sgc::graphics::PixelSize2D{tileSize.x, tileSize.y});

                m_mapView->SetCursorSizeInPixels({
                    selection.x,
                    selection.y
                });

                m_mapView->SetCursorPositionInPixels({
                    x_tile,
                    y_tile
                });
                
                shouldUpdate = true;                
            }

            if (!m_isPainting){
                if(shouldUpdate) {
                    Update();
                }
                break;
            }                

            auto cursorTileSize = m_mapView->GetCursorSizeInTiles();

            auto cursorTilePositionOnTileset = m_mapView->PixelsToTiles(                
                tilesetSection->GetCursorPositionInPixels().value_or(sgc::graphics::PixelPosition2D{0, 0})
            );

            auto cursorPositionOnMap = m_mapView->GetCursorPositionInTiles();

            m_brush.PaintExecuteChange(
                *mapDocument,
                *m_mapView->GetTileset(),
                cursorPositionOnMap,
                cursorTilePositionOnTileset,
                cursorTileSize
            );

            Update();

            return 0;
        }

        case WM_LBUTTONUP:
        {
            if(m_isPainting) {
                m_isPainting = false;

                m_brush.PaintCommitChanges(*mapDocument);

                ReleaseCaptureHelper();
            }

            return 0;
        }

        case WM_MBUTTONUP:
        {
            if (m_isPanning)
            {
                m_isPanning = false;
                ReleaseCaptureHelper();
            }

            return 0;
        }

        case WM_MBUTTONDOWN:
        {
            if(IsMouseCaptured()) {
                break;
            }

            m_isPanning = true;

            m_lastMousePosPan.x = static_cast<sgc::math::ival>(GET_X_LPARAM(lparam));
            m_lastMousePosPan.y = static_cast<sgc::math::ival>(GET_Y_LPARAM(lparam));

            SetCaptureHelper(hwnd);

            return 0;
        }
    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

void sections::MapSection::SetCheckTileBeforePainting(bool check)
{
    m_brush.SetCheckTileBeforePainting(check);
}

void sections::MapSection::SetPaintMode(editor_tools::PaintMode paintMode)
{
    m_brush.SetPaintMode(paintMode);
}
#include "win32_program/windows_init.hpp"
#undef CreateWindow // avoid macro name conflict with sdl::CreateWindow
#include "win32_program/windows_controls.hpp"
#include "program/program.hpp"

program::ProgramContext& program::GetProgramContext()
{
    static ProgramContext context = {};
    return context;
}

void program::StartEditor(std::wstring tilesetPath, int tileWidth, int tileHeight, int chunksSizeX, int chunksSizeY)
{
    (void)chunksSizeX;
    (void)chunksSizeY;
    
    auto& programContext = GetProgramContext();
    auto& win32Context = win32_program::GetWin32Context();

    if(programContext.tilesetSection == nullptr || win32Context.hMapView == nullptr) {
        return;
    }

    if(programContext.selectionRectangleOnTileset != nullptr) {
        programContext.selectionRectangleOnTileset.reset();
    }

    if(programContext.tilesetView != nullptr) {
        programContext.tilesetView.reset();
    }

    if(programContext.mapView != nullptr) {
        programContext.mapView.reset();
    }

    programContext.selectionRectangleOnTileset = std::make_unique<sgc::graphics::Rectangle>(
        sgc::math::vec2{ 0, 0 },
        sgc::math::uvec2{ static_cast<sgc::math::uval>(tileWidth), static_cast<sgc::math::uval>(tileHeight) }
    );
    programContext.selectionRectangleOnTileset->SetColor({ 0, 128, 255, 128 });

    /* programContext.tilesetView = std::make_unique<sgc_view::TilesetView>(win32Context.hTilesetView, tileWidth, tileHeight);
    programContext.tilesetView->LoadTileset(tilesetPath); */    
    programContext.tilesetSection->LoadTileset(tilesetPath);
    win32Context.hTilesetView = programContext.tilesetSection->GetHwnd();

    /* programContext.mapView = std::make_unique<sgc_view::MapView>(win32Context.hMapView, tileWidth, tileHeight);
    programContext.mapView->LoadTileset(tilesetPath); */
    programContext.mapSection->LoadTileset(tilesetPath);
    win32Context.hMapView = programContext.mapSection->GetHwnd();

    win32_program::UpdateToolbar(win32Context);
}

void program::HandleResize()
{
    auto& programContext = GetProgramContext();
    /* if (programContext.tilesetView != nullptr) {
        RECT rect;
        GetClientRect(win32_program::GetWin32Context().hTilesetView, &rect);
        programContext.tilesetView->SetScreenSize(rect.right - rect.left, rect.bottom - rect.top);
        programContext.tilesetView->Render();        
    } */

    if (programContext.tilesetSection != nullptr) {
        programContext.tilesetSection->HandleSectionResize();
        programContext.tilesetSection->Update();
    }

    if (programContext.mapSection != nullptr) {
        programContext.mapSection->HandleSectionResize();
        programContext.mapSection->Update();
    }
}
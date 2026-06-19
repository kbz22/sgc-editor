#include "win32_program/windows_init.hpp"
#undef CreateWindow // avoid macro name conflict with sdl::CreateWindow
#include "program/program.hpp"

program::ProgramContext& program::GetProgramContext()
{
    static ProgramContext context = {};
    return context;
}

void program::StartEditor(std::wstring tilesetPath)
{
    auto& programContext = GetProgramContext();
    auto& win32Context = win32_program::GetWin32Context();

    if(win32Context.hTilesetView == nullptr || win32Context.hMapView == nullptr) {
        return;
    }

    if(programContext.tilesetView != nullptr) {
        programContext.tilesetView.reset();
    }

    if(programContext.mapView != nullptr) {
        programContext.mapView.reset();
    }

    programContext.tilesetView = std::make_unique<sgc_view::TilesetView>(win32Context.hTilesetView);
    programContext.tilesetView->LoadTileset(tilesetPath);

    programContext.mapView = std::make_unique<sgc_view::MapView>(win32Context.hMapView);
    programContext.mapView->LoadTileset(tilesetPath);
}

void program::HandleResize()
{
    auto& programContext = GetProgramContext();
    if (programContext.tilesetView != nullptr) {
        programContext.tilesetView->Render();
    }
    if (programContext.mapView != nullptr) {
        programContext.mapView->Render();
    }
}
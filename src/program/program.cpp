#include "program/program.hpp"
#include "win32_program/windows_init.hpp"

program::ProgramContext& program::GetProgramContext()
{
    static ProgramContext context = {};
    return context;
}

void program::StartEditor(std::wstring tilesetPath)
{
    auto& programContext = GetProgramContext();
    auto& win32Context = win32_program::GetWin32Context();

    if(win32Context.hTilesetView == nullptr) {
        return;
    }

    if(programContext.tilesetView != nullptr) {
        programContext.tilesetView.reset();
    }

    programContext.tilesetView = std::make_unique<sgc::SgcView>(win32Context.hTilesetView);
    programContext.tilesetView->LoadTileset(tilesetPath);
}
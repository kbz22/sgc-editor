#include "win32_program/controls_fun.hpp"

#include "sgc/sgc_view.hpp"

#include "win32_program/windows_init.hpp"

void win32_program::OnMenuFileClicked()
{
}

void win32_program::OnFileNewClicked()
{
    auto& context = GetWin32Context();

    if (context.hTilesetView != nullptr) {
        sgc::SgcView view(context.hTilesetView);
        view.LoadTileset("test_icon.png", 16, 16);
    }
}
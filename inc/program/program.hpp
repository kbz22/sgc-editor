#pragma once

#include <string>
#include "sgc_view/tileset_view.hpp"

namespace program {

    struct ProgramContext
    {       
        std::unique_ptr<sgc_view::TilesetView> tilesetView;
    };

    ProgramContext& GetProgramContext();

    void StartEditor(std::wstring tilesetPath);
    void HandleResize();

}
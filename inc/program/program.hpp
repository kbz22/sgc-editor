#pragma once

#include <string>
#include "sgc/tileset_view.hpp"

namespace program {

    struct ProgramContext
    {       
        std::unique_ptr<sgc::TilesetView> tilesetView;
    };

    ProgramContext& GetProgramContext();

    void StartEditor(std::wstring tilesetPath);
    void HandleResize();

}
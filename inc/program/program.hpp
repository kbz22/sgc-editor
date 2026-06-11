#pragma once

#include <string>
#include "sgc/sgc_view.hpp"

namespace program {

    struct ProgramContext
    {       
        std::unique_ptr<sgc::SgcView> tilesetView;
    };

    ProgramContext& GetProgramContext();

    void StartEditor(std::wstring tilesetPath);
    void HandleResize();

}
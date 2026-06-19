#pragma once

#include <string>
#include "sgc_view/tileset_view.hpp"
#include "sgc_view/map_view.hpp"

namespace program {

    struct ProgramContext
    {       
        std::unique_ptr<sgc_view::TilesetView> tilesetView;
        std::unique_ptr<sgc_view::MapView> mapView;
    };

    ProgramContext& GetProgramContext();

    void StartEditor(std::wstring tilesetPath);
    void HandleResize();

}
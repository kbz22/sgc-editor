#pragma once

#include <string>
#include "sgc_view/tileset_view.hpp"
#include "sgc_view/map_view.hpp"
#include <sgc/graphics/rectangle.hpp>

namespace program {

    struct ProgramContext
    {       
        std::unique_ptr<sgc_view::TilesetView> tilesetView;
        std::unique_ptr<sgc_view::MapView> mapView;
        std::unique_ptr<sgc::graphics::Rectangle> selectionRectangleOnTileset; 
    };

    ProgramContext& GetProgramContext();

    void StartEditor(std::wstring tilesetPath, int tileWidth, int tileHeight, int chunksSizeX, int chunksSizeY);
    void HandleResize();

}
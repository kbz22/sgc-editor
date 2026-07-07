#pragma once

#include <string>
#include "sgc_view/tileset_view.hpp"
#include "sgc_view/map_view.hpp"
#include <sgc/graphics/rectangle.hpp>

#include "win32_section/tileset_section.hpp"
#include "win32_section/map_section.hpp"

namespace program {

    struct ProgramContext
    {       
        std::unique_ptr<sgc_view::TilesetView> tilesetView;
        std::unique_ptr<sgc_view::MapView> mapView;
        std::unique_ptr<sgc::graphics::Rectangle> selectionRectangleOnTileset; 

        std::unique_ptr<win32_section::TilesetSection> tilesetSection;
        std::unique_ptr<win32_section::MapSection> mapSection;
    };

    ProgramContext& GetProgramContext();

    void StartEditor(std::wstring tilesetPath, int tileWidth, int tileHeight, int chunksSizeX, int chunksSizeY);
    void HandleResize();

}
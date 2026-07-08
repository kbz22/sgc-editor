#pragma once

#include <string>
#include "sgc_view/tileset_view.hpp"
#include "sgc_view/map_view.hpp"
#include <sgc/graphics/rectangle.hpp>

#include "sections/tileset_section.hpp"
#include "sections/map_section.hpp"
#include "sections/toolbar_section.hpp"

#include "program/program_state.hpp"

namespace program {    

    struct ProgramContext
    {
        std::unique_ptr<sections::TilesetSection> tilesetSection;        
        std::unique_ptr<sections::MapSection> mapSection;
        std::unique_ptr<sections::ToolbarSection> toolbarSection;

        std::vector<sections::Section*> sections;

        std::unique_ptr<sgc::graphics::Rectangle> selectionRectangleOnTileset;

        EditorState state = EditorState::Default;
        std::wstring currentProjectName;
    };

    ProgramContext& GetProgramContext();

    void StartEditor(std::wstring tilesetPath, int tileWidth, int tileHeight, int chunksSizeX, int chunksSizeY);
    void HandleResize();

}
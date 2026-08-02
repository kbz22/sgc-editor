#pragma once

#include "program/editor_mode.hpp"
#include "editor_tools/brush.hpp"

namespace program {

    void UpdateEditorLayerMode(program::EditorLayerMode newMode);
    void UpdateEditorChunkMode(program::EditorChunkMode newMode);
    void UpdateBrushMode(editor_tools::PaintMode newMode);

}
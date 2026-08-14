#pragma once

#include "program/editor_mode.hpp"
#include "editor_tools/brush.hpp"

namespace program {

    void UpdateEditorLayerMode(program::EditorLayerMode newMode, program::ProgramContext& programContext);
    void UpdateEditorChunkMode(program::EditorChunkMode newMode, program::ProgramContext& programContext);
    void UpdateBrushMode(editor_tools::PaintMode newMode, program::ProgramContext& programContext);
    void UpdateBrushEraseMode(editor_tools::EraserMode newMode, program::ProgramContext& programContext);
    void UpdateEditorGridMode(program::EditorGridMode newMode, program::ProgramContext& programContext);
    void UpdateEditorSelectionMode(editor_tools::SelectionMode newMode, program::ProgramContext& programContext);
    void UpdateEditorSelectionTools(program::ProgramContext& programContext);

}
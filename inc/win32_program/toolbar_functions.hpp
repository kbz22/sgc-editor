#pragma once

#include "program/editor_mode.hpp"

namespace win32_program
{    
    void OnFileNewClicked();
    void OnFileSaveClicked();
    void OnFileOpenClicked();

    void OnEditUndoClicked();
    void OnEditRedoClicked();

    void OnLayerAddClicked();
    void OnLayerRemoveClicked();
    void OnLayerMoveUpClicked();
    void OnLayerMoveDownClicked();

    void UpdateEditorLayerMode(program::EditorLayerMode newMode);
    void UpdateEditorChunkMode(program::EditorChunkMode newMode);
}
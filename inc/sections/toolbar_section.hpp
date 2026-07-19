#pragma once

#include "sections/section.hpp"
#include "win32_models/toolbarbutton.hpp"
#include "win32_program/windows_controls.hpp"

#include <windows.h>
#include <vector>

namespace program {
    struct ProgramContext;
}

namespace sections {

    class ToolbarSection : public Section
    {
        private:            
            /* std::vector<win32_models::ToolbarButton> m_buttons = {
                {0, win32_program::CommandId::FileNew, true},
                {1, win32_program::CommandId::FileOpen, true},
                {2, win32_program::CommandId::FileSave, false},
                {0, win32_program::CommandId::NotApplicable, false, true},
                {11, win32_program::CommandId::EditUndo, true},
                {12, win32_program::CommandId::EditRedo, true},
                {0, win32_program::CommandId::NotApplicable, false, true},
                {7, win32_program::CommandId::LayerAdd, true},
                {8, win32_program::CommandId::LayerRemove, true},
                {10, win32_program::CommandId::LayerMoveUp, true},
                {9, win32_program::CommandId::LayerMoveDown, true},
                {0, win32_program::CommandId::NotApplicable, false, true},
                {4, win32_program::CommandId::EditorLayerModeNonActiveTransparent, true, false, true},
                {5, win32_program::CommandId::EditorLayerModeSingleLayer, true, false, true},
                {6, win32_program::CommandId::EditorLayerModeSingleImage, true, false, true},
                {0, win32_program::CommandId::NotApplicable, false, true},
                {16, win32_program::CommandId::EditorChunkModeFixedSize, true, false, true},
                {15, win32_program::CommandId::EditorChunkModeFree, true, false, true}
            }; */
            HWND m_hwndToolbar;
            program::ProgramContext& m_programContext;

        public:
            ToolbarSection(program::ProgramContext& programContext);
            ~ToolbarSection();

            void Update() override;            
            void HandleSectionResize() override;

            void Refresh();            

            HWND GetHwndToolbar() const;
    };

}
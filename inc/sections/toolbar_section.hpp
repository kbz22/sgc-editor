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
            std::vector<win32_models::ToolbarButton> m_buttons = {
                {0, win32_program::CommandId::FileNew,  L"New File", true},
                {1, win32_program::CommandId::FileOpen, L"Open File", true},
                {2, win32_program::CommandId::FileSave, L"Save File", false},
                {11, win32_program::CommandId::EditUndo, L"Undo", true},
                {12, win32_program::CommandId::EditRedo, L"Redo", true},
                {7, win32_program::CommandId::LayerAdd, L"Add Layer", true},
                {8, win32_program::CommandId::LayerRemove, L"Remove Layer", true},
                {10, win32_program::CommandId::LayerMoveUp, L"Move Layer Up", true},
                {9, win32_program::CommandId::LayerMoveDown, L"Move Layer Down", true}
            };
            HWND m_hwndToolbar;

        public:
            ToolbarSection(program::ProgramContext& programContext);
            ~ToolbarSection();

            void Update() override;
            void HandleSectionResize() override;

            HWND GetHwndToolbar() const;
    };

}
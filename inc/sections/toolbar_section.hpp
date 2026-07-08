#pragma once

#include "sections/section.hpp"
#include "win32_models/toolbarbutton.hpp"
#include "win32_program/windows_controls.hpp"

#include <windows.h>
#include <vector>

namespace sections {

    class ToolbarSection : public Section
    {
        private:            
            std::vector<win32_models::ToolbarButton> m_buttons = {
                {0, win32_program::CommandId::FileNew,  L"New File", true},
                {1, win32_program::CommandId::FileOpen, L"Open File", true},
                {2, win32_program::CommandId::FileSave, L"Save File", false}
            };
            HWND m_hwndToolbar;

        public:
            ToolbarSection(win32_program::Win32Context& context);
            ~ToolbarSection();

            void Update() override;
            void HandleSectionResize() override;

            HWND GetHwndToolbar() const;
    };

}
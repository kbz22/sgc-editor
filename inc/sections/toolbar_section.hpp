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
            HWND m_hwndToolbar;            

        public:
            ToolbarSection(program::ProgramContext& programContext);
            ~ToolbarSection();

            void Update() override;            
            void HandleSectionResize() override;

            void Refresh(program::ProgramContext& programContext);

            HWND GetHwndToolbar() const;
    };

}
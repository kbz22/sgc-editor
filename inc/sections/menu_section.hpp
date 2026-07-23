#pragma once

#include "sections/section.hpp"
#include "win32_program/win32_context.hpp"
#include <vector>
#include <windows.h>

namespace program {
    struct ProgramContext;
}

namespace sections {

    class MenuSection : public Section
    {
        private:
            HWND m_hwndToolbar = HWND();
            HMENU m_filePopupMenu = nullptr;

        public:
            MenuSection(program::ProgramContext& programContext);
            ~MenuSection();

            void Update() override;
            void HandleSectionResize() override;

            HWND GetHwndToolbar() const;
    };

}
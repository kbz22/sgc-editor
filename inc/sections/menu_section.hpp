#pragma once

#include "sections/section.hpp"
#include <vector>
#include <windows.h>

namespace program {
    class ProgramContext;
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
            void Refresh(program::ProgramContext& programContext) override;
    };

}
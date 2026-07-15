#pragma once

#include "sections/section.hpp"
#include "win32_models/menu_item.hpp"
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
            std::vector<win32_models::MenuItem> m_menuItems;
            HWND m_hwndToolbar = HWND();

        public:
            MenuSection(program::ProgramContext& programContext);
            ~MenuSection();

            void Update() override;
            void HandleSectionResize() override;

            HWND GetHwndToolbar() const;
    };

}
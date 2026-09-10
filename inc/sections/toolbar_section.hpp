#pragma once

#include "sections/section.hpp"
#include "action/widget_action.hpp"
#include "win32_program/windows_controls.hpp"

#include <vector>
#include <windows.h>

namespace program {
    class ProgramContext;
}

namespace sections {

    class ToolbarSection : public Section
    {
        private:
            HWND m_hwndToolbar;
            std::vector<action::WidgetAction*> m_widgets;

        public:
            ToolbarSection(program::ProgramContext& programContext);
            ~ToolbarSection();

            void Update() override;
            void HandleSectionResize() override;

            void Refresh(program::ProgramContext& programContext) override;

            HWND GetHwndToolbar() const;

            constexpr static int g_ButtonPadding = 12;
            constexpr static int g_ButtonSize = 32;
            constexpr static int g_ButtonBitmapSize = 24;
    };

}
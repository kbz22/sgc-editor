#pragma once

#include <windows.h>
#include "win32_program/win32_context.hpp"
#include "win32_program/windows_controls.hpp"

namespace win32_section {

    class Section
    {
        private:
            HWND m_hwnd;
            HWND m_parentHwnd;

        public:
            Section(LPCWSTR name, win32_program::ControlId id, win32_program::Win32Context& context);
            ~Section();

            virtual void Update() = 0;
            virtual void HandleSectionResize() = 0;

            HWND GetHwnd() const;            
    };

}
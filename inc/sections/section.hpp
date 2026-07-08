#pragma once

#include <windows.h>
#include "win32_program/win32_context.hpp"
#include "win32_program/windows_controls.hpp"
#include "program/program_state.hpp"

namespace sections {

    class Section
    {
        private:
            HWND m_hwnd = HWND();
            HWND m_parentHwnd = HWND();

        protected:
            void SetHwnd(HWND hwnd, HWND parentHwnd);
            Section();

        public:
            Section(LPCWSTR name, win32_program::ControlId id, win32_program::MainWindowContext& context);
            ~Section();

            virtual void Update() = 0;
            virtual void HandleSectionResize() = 0;

            HWND GetHwnd() const;            
    };

}
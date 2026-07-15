 #pragma once

#include <windows.h>
#include "win32_program/win32_context.hpp"
#include "win32_program/windows_controls.hpp"
#include "sgc_view/sgc_view.hpp"
#include <sgc/sdl/sdl.hpp>

namespace sections {

    class Section
    {
        private:
            HWND m_hwnd = HWND();
            HWND m_parentHwnd = HWND();
            
            static LRESULT CALLBACK StaticPaneProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR id, DWORD_PTR data);

        protected:
            void SetHwnd(HWND hwnd, HWND parentHwnd);
            Section();

            virtual LRESULT HandleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
            void AttachView(sgc_view::SgcView& view);
            void Redraw();

        public:
            Section(LPCWSTR name, win32_program::ControlId id, win32_program::MainWindowContext& context);
            ~Section();

            virtual void Update() = 0;
            virtual void HandleSectionResize() = 0;            

            HWND GetHwnd() const;            
    };

}
 #pragma once

#include <windows.h>
#include "win32_program/windows_controls.hpp"
#include "sgc_view/sgc_view.hpp"
#include <sgc/sdl/sdl.hpp>

namespace sections {

    class Section
    {
        private:
            HWND m_hwnd = HWND();
            HWND m_parentHwnd = HWND();

            static bool m_registered;
            
            static LRESULT CALLBACK StaticPaneProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR id, DWORD_PTR data);
            static LRESULT CALLBACK DefaultSectionProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

        protected:
            void SetHwnd(HWND hwnd, HWND parentHwnd);
            Section();

            virtual LRESULT HandleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
            void AttachView(sgc_view::SgcView& view);
            void SetupSubclass(HWND hwnd, Section* section);
            void Redraw();

        public:
            Section(LPCWSTR name, win32_program::ControlId id, HWND parentHwnd, HINSTANCE hInstance);
            ~Section();

            virtual void Update() = 0;
            virtual void Refresh(program::ProgramContext& programContext) = 0;
            virtual void HandleSectionResize() = 0;

            HWND GetHwnd() const;
    };    

}
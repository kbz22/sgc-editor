#pragma once

#include <windows.h>
#include <string>
#include <functional>
#include <optional>

namespace win32_models {

    class EditTextBox
    {
        private:
            HWND m_hwnd = HWND();
            int m_x = 0;
            int m_y = 0;
            int m_width = 200;
            int m_height = 16;            
            bool m_visible = false;
            std::function<void(std::wstring)> m_onEditingFinished = nullptr;

            static LRESULT CALLBACK EditTextBoxStaticProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR id, DWORD_PTR data);
            LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

        public:
            EditTextBox(HWND hwndParent, HINSTANCE hInstance);
            ~EditTextBox();

            void SetSize(int width, int height);
            void StartEditing(
                int x,
                int y,
                std::wstring initialText = L"",                
                std::function<void(std::wstring)> onEditingFinished = nullptr
            );
            void Update();

            HWND GetHwnd() const;
            int GetWidth() const;
    };

}
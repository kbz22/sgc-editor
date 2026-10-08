#pragma once

#include <windows.h>

namespace win32_program
{
    class DpiManager
    {
        private:
            UINT m_dpi;            

        public:
            DpiManager(HWND hwnd);

            void SetDpi(UINT dpi);

            UINT GetDpi() const;

            int Scale(int value) const;
    };
}
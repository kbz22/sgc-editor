#pragma once

#include <windows.h>

namespace win32_models {

    class IWidget
    {
        public:
            virtual ~IWidget() = default;

            virtual int GetWidth() const = 0;            
            virtual HWND GetHWND() const = 0;
            virtual void SetPosition(int x, int y) = 0;
            virtual void Update() = 0;
    };

}
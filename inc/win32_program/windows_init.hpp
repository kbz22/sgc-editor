#pragma once

#include <windows.h>
#include <string>

namespace win32_program
{   

    void Init(HINSTANCE hInstance);

    void Run();

    void SetTitle(HWND hwnd, const std::wstring& title);

    void SetWindowPositionAndSize(HWND hwnd, int x, int y, int width, int height);

}

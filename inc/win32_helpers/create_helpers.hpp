#pragma once

#include <windows.h>
#include "types.hpp"

#pragma comment(lib, "comctl32.lib")

namespace win32_helpers
{
    HWND CreateToolbar(HWND hwndParent, HINSTANCE hInstance, types::ctrid_t id);    
    HWND CreateRebar(HWND hwndParent, HINSTANCE hInstance, types::ctrid_t id);
}
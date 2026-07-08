#pragma once

#include <windows.h>

namespace win32_program {

    struct Win32Context
    {
        HINSTANCE hInstance = nullptr;

        HWND hMainWindow = HWND();

        HWND hMapView = HWND();
        HWND hLayerListView = HWND();
        HWND hPackageView = HWND();
        HWND hTilesetView = HWND();

        HWND hSplitLeft = HWND();
        HWND hSplitRight = HWND();
        HWND hSplitBottom = HWND();

        HWND hRebarTop = HWND();        
        HWND hToolbarMenu = HWND();
        HWND hRebarBottom = HWND();
        HWND hToolbarFunctions = HWND();
    };

    Win32Context& GetWin32Context();

}
#include "win32_helpers/create_helpers.hpp"

#include <windows.h>
#include <commctrl.h>

HWND win32_helpers::CreateToolbar(HWND hwndParent, HINSTANCE hInstance, types::ctrid_t id)
{
    HWND hToolbar = CreateWindowEx(
        0, TOOLBARCLASSNAME, nullptr,
        WS_CHILD | WS_VISIBLE |
        CCS_NODIVIDER |
        TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | TBSTYLE_LIST |
        CCS_NORESIZE,
        0, 0, 0, 0,
        hwndParent, (HMENU)id, hInstance, nullptr
    );

    SendMessage(hToolbar, TB_SETPADDING, 0, MAKELPARAM(2, 2));

    return hToolbar;
}

HWND win32_helpers::CreateRebar(HWND hwndParent, HINSTANCE hInstance, types::ctrid_t id)
{
    HWND hRebar = CreateWindowEx(0, REBARCLASSNAME, nullptr,
            RBS_BANDBORDERS | RBS_DBLCLKTOGGLE | RBS_REGISTERDROP | RBS_VARHEIGHT |
            CCS_NODIVIDER | 
            WS_BORDER | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
            0, 0, 0, 0, hwndParent, (HMENU)id, hInstance, nullptr);

    return hRebar;
}

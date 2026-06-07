#include "win32_program/control_setup.hpp"
#include "win32_helpers/create_helpers.hpp"
#include "win32_helpers/load_bitmap.hpp"

void win32_program::SetupMenuBar(Win32Context &context)
{
    using namespace win32_program;

    context.hRebarTop = win32_helpers::CreateRebar(
        context.hMainWindow,
        context.hInstance,
        static_cast<types::ctrid_t>(ControlId::Menu)
    );

    context.hToolbarMenu = win32_helpers::CreateToolbar(
        context.hMainWindow,
        context.hInstance,
        static_cast<types::ctrid_t>(ControlId::Toolbar)
    );

    // Required for TBADDBUTTONS to work correctly
    SendMessage(context.hToolbarMenu, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);

    int strIndex = static_cast<int>(
        SendMessage(
            context.hToolbarMenu,
            TB_ADDSTRING,
            0,
            (LPARAM)L"File"
        )
    );

    TBBUTTON btn = {};
    btn.iBitmap = I_IMAGENONE;
    btn.idCommand = static_cast<int>(CommandId::MenuFile);
    btn.fsState = TBSTATE_ENABLED;
    btn.fsStyle = BTNS_BUTTON | BTNS_SHOWTEXT | BTNS_DROPDOWN;
    btn.iString = strIndex;

    SendMessage(context.hToolbarMenu, TB_ADDBUTTONS, 1, (LPARAM)&btn);    
    SendMessage(context.hToolbarMenu, TB_AUTOSIZE, 0, 0);

    SIZE sz = {};
    SendMessage(context.hToolbarMenu, TB_GETMAXSIZE, 0, (LPARAM)&sz);

    REBARBANDINFO rb = { sizeof(rb) };
    rb.fMask = RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE;
    rb.hwndChild = context.hToolbarMenu;
    rb.cxMinChild = sz.cx;
    rb.cyMinChild = sz.cy;
    rb.fStyle = RBBS_CHILDEDGE;

    SendMessage(context.hRebarTop, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rb);
}

void win32_program::SetupToolbar(Win32Context &context)
{
    using namespace win32_program;

    context.hRebarBottom = win32_helpers::CreateRebar(
        context.hMainWindow,
        context.hInstance,
        static_cast<types::ctrid_t>(ControlId::Menu)
    );

    context.hToolbarFunctions = win32_helpers::CreateToolbar(
        context.hMainWindow,
        context.hInstance,
        static_cast<types::ctrid_t>(ControlId::Toolbar)
    );

    LONG style = static_cast<LONG>(SendMessage(context.hToolbarFunctions, TB_GETSTYLE, 0, 0)) | TBSTYLE_FLAT;
    SendMessage(context.hToolbarFunctions, TB_SETSTYLE, 0, style);

    SendMessage(context.hToolbarFunctions, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);

    HIMAGELIST img = ImageList_Create(24, 24, ILC_COLOR32, 10, 0);
    HBITMAP hBmp = win32_helpers::LoadPngWIC(L"./testicon2.png");
    
    ImageList_Add(img, hBmp, NULL);

    SendMessage(context.hToolbarFunctions, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);        
    SendMessage(context.hToolbarFunctions, TB_SETIMAGELIST, 0, (LPARAM)img);

    TBBUTTON btn = { 
        0,
        static_cast<int>(CommandId::FileNew),
        TBSTATE_ENABLED,
        BTNS_BUTTON,
        {0},
        0,
        -1
    };

    btn.dwData = (DWORD_PTR)L"New File";

    SendMessage(context.hToolbarFunctions, TB_ADDBUTTONS, 1, (LPARAM)&btn);    
    SendMessage(context.hToolbarFunctions, TB_SETBUTTONSIZE, 0, MAKELPARAM(30, 30));

    SendMessage(context.hToolbarFunctions, TB_AUTOSIZE, 0, 0);

    SIZE sz = {};
    SendMessage(context.hToolbarFunctions, TB_GETMAXSIZE, 0, (LPARAM)&sz);

    REBARBANDINFO rb = { sizeof(rb) };
    rb.fMask = RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE;
    rb.hwndChild = context.hToolbarFunctions;
    rb.cxMinChild = 24;
    rb.cyMinChild = sz.cy;
    rb.fStyle = RBBS_CHILDEDGE;

    SendMessage(context.hRebarBottom, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rb);

    return;
}

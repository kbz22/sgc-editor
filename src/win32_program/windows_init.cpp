#include "win32_program/windows_controls.hpp"
#include "win32_program/window_control.hpp"
#include "win32_program/controls_fun.hpp"
#include "win32_program/windows_process.hpp"
#include "defaults.hpp"

#undef CreateWindow // avoid macro name conflict with sdl::CreateWindow

#include <windowsx.h>
#include <algorithm>

void win32_program::Run()
{
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

win32_program::Win32Context& win32_program::GetWin32Context()
{
    static Win32Context context = {};
    return context;
}

void win32_program::Init(HINSTANCE hInstance, Win32Context &context)
{    
    context.hInstance = hInstance;

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"SGCEditorMainWindow";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        L"SGC Editor",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT,
        defaults::WindowWidth,
        defaults::WindowHeight,
        nullptr, nullptr, hInstance, nullptr
    );    

    return;
}

#include "win32_helpers/create_helpers.hpp"
#include "win32_helpers/load_bitmap.hpp"
#include "win32_helpers/file_helpers.hpp" 
#include "win32_models/toolbarbutton.hpp"   

void win32_program::SetupMenuBar(Win32Context &context)
{
    using namespace win32_program;    ;

    context.hRebarTop = win32_helpers::CreateRebar(
        context.hMainWindow,
        context.hInstance,
        static_cast<types::ctrid_t>(ControlId::Menu)
    );

    context.hToolbarMenu = win32_helpers::CreateToolbar(
        context.hRebarTop,
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
        context.hRebarBottom,
        context.hInstance,
        static_cast<types::ctrid_t>(ControlId::Toolbar)
    );

    LONG style = static_cast<LONG>(SendMessage(context.hToolbarFunctions, TB_GETSTYLE, 0, 0)) | TBSTYLE_FLAT;
    SendMessage(context.hToolbarFunctions, TB_SETSTYLE, 0, style);

    SendMessage(context.hToolbarFunctions, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);

    HIMAGELIST img = ImageList_Create(24, 24, ILC_COLOR32, 10, 0);
    HBITMAP hBmp = win32_helpers::LoadPngWIC(L"./testicon2.png");

    HIMAGELIST imgDisabled = ImageList_Create(24, 24, ILC_COLOR32, 10, 0);
    HBITMAP hBmpDisabled = win32_helpers::LoadPngWIC(L"./testicon2_disabled.png");
    
    ImageList_Add(img, hBmp, NULL);
    ImageList_Add(imgDisabled, hBmpDisabled, NULL);

    SendMessage(context.hToolbarFunctions, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);        
    SendMessage(context.hToolbarFunctions, TB_SETIMAGELIST, 0, (LPARAM)img);
    SendMessage(context.hToolbarFunctions, TB_SETDISABLEDIMAGELIST, 0, (LPARAM)imgDisabled);

    std::vector<win32_models::ToolbarButton> buttons =
    {
        {0, CommandId::FileNew,  L"New File", true},
        {1, CommandId::FileOpen, L"Open File", true},
        {2, CommandId::FileSave, L"Save File", false}
    };

    std::vector<TBBUTTON> tbButtons;
    tbButtons.reserve(buttons.size());

    for (const auto& b : buttons)
    {
        TBBUTTON btn = {};

        btn.iBitmap = b.imageIndex;
        btn.idCommand = static_cast<int>(b.commandId);
        btn.fsState = b.enabled ? TBSTATE_ENABLED : 0;
        btn.fsStyle = BTNS_BUTTON;

        btn.dwData = (DWORD_PTR)L"New File"; //! tmp

        tbButtons.push_back(btn);
    }

    SendMessage(context.hToolbarFunctions, TB_ADDBUTTONS,
            (WPARAM)tbButtons.size(),
            (LPARAM)tbButtons.data());     

    SendMessage(context.hToolbarFunctions, TB_SETBUTTONSIZE, 0, MAKELPARAM(30, 30));
    SendMessage(context.hToolbarFunctions, TB_AUTOSIZE, 0, 0);

    SIZE sz = {};
    SendMessage(context.hToolbarFunctions, TB_GETMAXSIZE, 0, (LPARAM)&sz);

    REBARBANDINFO rb = { sizeof(rb) };
    rb.fMask = RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE;
    rb.hwndChild = context.hToolbarFunctions;
    rb.cxMinChild = sz.cx;
    rb.cyMinChild = sz.cy;
    rb.fStyle = RBBS_CHILDEDGE;

    SendMessage(context.hRebarBottom, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rb);

    return;
}

#include "program/program.hpp"

void win32_program::UpdateToolbar(Win32Context &context)
{
    program::ProgramContext& programContext = program::GetProgramContext();

    bool hasMap = (programContext.mapView != nullptr);

    SendMessage(
        context.hToolbarFunctions,
        TB_ENABLEBUTTON,
        static_cast<WPARAM>(CommandId::FileSave),
        MAKELONG(hasMap, 0));
}

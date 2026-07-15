#include "sections/menu_section.hpp"
#include "win32_helpers/create_helpers.hpp"
#include <windows.h>
#include <commctrl.h>

sections::MenuSection::MenuSection(win32_program::MainWindowContext& context)    
{
    using namespace win32_program;

    auto hwndRebar = win32_helpers::CreateRebar(
        context.hMainWindow,
        context.hInstance,
        static_cast<types::ctrid_t>(ControlId::MenuRebar)
    );

    auto hwndToolbar = win32_helpers::CreateToolbar(
        hwndRebar,
        context.hInstance,
        static_cast<types::ctrid_t>(ControlId::MenuToolbar)
    );

    // Required for TBADDBUTTONS to work correctly
    SendMessage(hwndToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);

    int strIndex = static_cast<int>(
        SendMessage(
            hwndToolbar,
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

    SendMessage(hwndToolbar, TB_ADDBUTTONS, 1, (LPARAM)&btn);    
    SendMessage(hwndToolbar, TB_AUTOSIZE, 0, 0);

    SIZE sz = {};
    SendMessage(hwndToolbar, TB_GETMAXSIZE, 0, (LPARAM)&sz);

    REBARBANDINFO rb = { sizeof(rb) };
    rb.fMask = RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE;
    rb.hwndChild = hwndToolbar;
    rb.cxMinChild = sz.cx;
    rb.cyMinChild = sz.cy;
    rb.fStyle = RBBS_CHILDEDGE;

    SendMessage(hwndRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rb);

    SetHwnd(hwndRebar, context.hMainWindow);
    m_hwndToolbar = hwndToolbar;
}

sections::MenuSection::~MenuSection()
{
    if (m_hwndToolbar)
    {
        DestroyWindow(m_hwndToolbar);
        m_hwndToolbar = nullptr;
    }
}

void sections::MenuSection::Update()
{
    Redraw();
    return;
}

void sections::MenuSection::HandleSectionResize()
{
    return;
}

HWND sections::MenuSection::GetHwndToolbar() const
{
    return m_hwndToolbar;
}
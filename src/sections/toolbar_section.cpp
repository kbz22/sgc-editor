#include "sections/toolbar_section.hpp"
#include "win32_helpers/create_helpers.hpp"
#include "win32_helpers/load_bitmap.hpp"
#include "program/program.hpp"

#include <windowsx.h>
#include <commctrl.h>

sections::ToolbarSection::ToolbarSection(program::ProgramContext& programContext)
{
    using namespace win32_program;

    MainWindowContext &context = *programContext.mainWindowContext;

    auto hwndRebar = win32_helpers::CreateRebar(
        context.hMainWindow,
        context.hInstance,
        static_cast<types::ctrid_t>(ControlId::ToolbarRebar)
    );

    auto hwndToolbar = win32_helpers::CreateToolbar(
        hwndRebar,
        context.hInstance,
        static_cast<types::ctrid_t>(ControlId::ToolbarToolbar)
    );

    LONG style = static_cast<LONG>(SendMessage(hwndToolbar, TB_GETSTYLE, 0, 0)) | TBSTYLE_FLAT;
    SendMessage(hwndToolbar, TB_SETSTYLE, 0, style);

    SendMessage(hwndToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);

    SendMessage(hwndToolbar, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);        
    SendMessage(hwndToolbar, TB_SETIMAGELIST, 0, (LPARAM)programContext.toolbarIcons);
    SendMessage(hwndToolbar, TB_SETDISABLEDIMAGELIST, 0, (LPARAM)programContext.toolbarIconsDisabled);

    std::vector<TBBUTTON> tbButtons;
    tbButtons.reserve(m_buttons.size());

    for (const auto& b : m_buttons)
    {
        TBBUTTON btn = {};

        btn.iBitmap = b.imageIndex;
        btn.idCommand = static_cast<int>(b.commandId);
        btn.fsState = b.enabled ? TBSTATE_ENABLED : 0;        

        btn.fsStyle = b.seperator ? BTNS_SEP : BTNS_BUTTON;
        btn.fsStyle = b.grouped ? BTNS_CHECKGROUP : btn.fsStyle;

        tbButtons.push_back(btn);
    }

    SendMessage(hwndToolbar, TB_ADDBUTTONS,
            (WPARAM)tbButtons.size(),
            (LPARAM)tbButtons.data());     

    SendMessage(hwndToolbar, TB_SETBUTTONSIZE, 0, MAKELPARAM(24, 24));
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

    return;

}

sections::ToolbarSection::~ToolbarSection()
{
    if (m_hwndToolbar)
    {
        DestroyWindow(m_hwndToolbar);
        m_hwndToolbar = nullptr;
    }
}

void sections::ToolbarSection::Update()
{
    Redraw();
    return;
}

void sections::ToolbarSection::HandleSectionResize()
{
    return;
}

void sections::ToolbarSection::SetGroupedButtonState(win32_program::CommandId commandId, bool checked)
{
    switch(commandId)
    {
        case win32_program::CommandId::EditorLayerModeNonActiveTransparent:
        case win32_program::CommandId::EditorLayerModeSingleLayer:
        case win32_program::CommandId::EditorLayerModeSingleImage:
        case win32_program::CommandId::EditorChunkModeFixedSize:
        case win32_program::CommandId::EditorChunkModeFree:
            SendMessage(m_hwndToolbar, TB_CHECKBUTTON, static_cast<int>(commandId), MAKELPARAM(checked, 0));
            break;
    }    
}

void sections::ToolbarSection::SetButtonEnabled(win32_program::CommandId commandId, bool enabled)
{
    SendMessage(m_hwndToolbar, TB_ENABLEBUTTON, static_cast<int>(commandId), MAKELPARAM(enabled, 0));
}

HWND sections::ToolbarSection::GetHwndToolbar() const
{
    return m_hwndToolbar;
}
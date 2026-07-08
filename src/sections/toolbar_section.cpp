#include "sections/toolbar_section.hpp"
#include "win32_helpers/create_helpers.hpp"
#include "win32_helpers/load_bitmap.hpp"

#include <windowsx.h>
#include <commctrl.h>

sections::ToolbarSection::ToolbarSection(win32_program::Win32Context& context)    
{
    using namespace win32_program;

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

    HIMAGELIST img = ImageList_Create(24, 24, ILC_COLOR32, 10, 0);
    HBITMAP hBmp = win32_helpers::LoadPngWIC(L"./testicon2.png");

    HIMAGELIST imgDisabled = ImageList_Create(24, 24, ILC_COLOR32, 10, 0);
    HBITMAP hBmpDisabled = win32_helpers::LoadPngWIC(L"./testicon2_disabled.png");
    
    ImageList_Add(img, hBmp, NULL);
    ImageList_Add(imgDisabled, hBmpDisabled, NULL);

    SendMessage(hwndToolbar, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);        
    SendMessage(hwndToolbar, TB_SETIMAGELIST, 0, (LPARAM)img);
    SendMessage(hwndToolbar, TB_SETDISABLEDIMAGELIST, 0, (LPARAM)imgDisabled);

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

    SendMessage(hwndToolbar, TB_ADDBUTTONS,
            (WPARAM)tbButtons.size(),
            (LPARAM)tbButtons.data());     

    SendMessage(hwndToolbar, TB_SETBUTTONSIZE, 0, MAKELPARAM(30, 30));
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
    return;
}

void sections::ToolbarSection::HandleSectionResize()
{
    return;
}

HWND sections::ToolbarSection::GetHwndToolbar() const
{
    return m_hwndToolbar;
}
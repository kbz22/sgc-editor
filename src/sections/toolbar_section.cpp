#include "sections/toolbar_section.hpp"
#include "win32_helpers/create_helpers.hpp"
#include "win32_helpers/load_bitmap.hpp"
#include "program/program.hpp"
#include "action/widget_action.hpp"
#include "win32_models/toolbarbutton.hpp"

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

    auto addSeperator = [](std::vector<TBBUTTON>& buttons) {
        TBBUTTON sepButton = {};
        sepButton.iBitmap = 0;
        sepButton.idCommand = static_cast<int>(action::ActionType::Default);
        sepButton.fsState = 0;
        sepButton.fsStyle = BTNS_SEP;
        buttons.push_back(sepButton);
    }; 
    
    auto toolbarActions = programContext.actionManager->GetToolbarActions();

    std::sort(toolbarActions.begin(), toolbarActions.end(), [](const action::Action* a, const action::Action* b) {
        return a->GetToolbarOrder() < b->GetToolbarOrder();
    });    

    std::vector<TBBUTTON> tbButtons;    
    action::GroupId lastGroupId = toolbarActions[0]->GetGroupId();    

    for (const auto& action : toolbarActions)
    {
        if (action->GetGroupId() != lastGroupId) {
            addSeperator(tbButtons);            
        }

        TBBUTTON btn = {};

        if(action->IsWidget())
        {
            auto widgetAction = dynamic_cast<action::WidgetAction*>(action);

            m_widgets.push_back(widgetAction);

            widgetAction->BuildWidget(
                hwndToolbar,
                programContext
            );

            btn.iBitmap = widgetAction->GetControlWidth();
            btn.idCommand = static_cast<int>(action->GetType());
            btn.fsState = 0;
            btn.fsStyle = 0;
        }
        else
        {
            btn.iBitmap = action->GetToolbarImageIndex();
            btn.idCommand = static_cast<int>(action->GetType());
            btn.fsState = action->IsEnabled() ? TBSTATE_ENABLED : 0;
            btn.fsStyle = action->IsCheckGroupItem() ? BTNS_CHECK : btn.fsStyle;   
        }
        
        action::GroupId groupId = action->GetGroupId();
        lastGroupId = groupId;
        tbButtons.push_back(btn);
    }

    SendMessage(hwndToolbar, TB_ADDBUTTONS,
            (WPARAM)tbButtons.size(),
            (LPARAM)tbButtons.data());    

    SendMessage(hwndToolbar, TB_SETPADDING, 0, MAKELPARAM(g_ButtonPadding, 0));
    SendMessage(hwndToolbar, TB_SETBITMAPSIZE, 0, MAKELPARAM(g_ButtonBitmapSize, g_ButtonBitmapSize));
    SendMessage(hwndToolbar, TB_SETBUTTONSIZE, 0, MAKELPARAM(g_ButtonSize, g_ButtonSize));
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

    SetupSubclass(hwndToolbar, this);

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
    for(auto widgetAction : m_widgets) {
        auto widget = widgetAction->GetWidget();
        auto commandId = widgetAction->GetType();

        int index = static_cast<int>(
        SendMessage(
            m_hwndToolbar,
            TB_COMMANDTOINDEX,
            static_cast<int>(commandId),
            0
        ));

        RECT rect{};

        SendMessage(
            m_hwndToolbar,
            TB_GETITEMRECT,
            index,
            reinterpret_cast<LPARAM>(&rect)
        );

        MapWindowPoints(
            m_hwndToolbar,
            GetHwnd(),
            reinterpret_cast<POINT*>(&rect),
            2
        );

        widget->SetPosition(rect.left, rect.top + 2);
        widget->Update();
    }
    Redraw();
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

void sections::ToolbarSection::Refresh(program::ProgramContext& programContext)
{
    auto toolbarActions = programContext.actionManager->GetToolbarActions();

    for (const auto& action : toolbarActions)
    {
        if(action->IsWidget())
        {
            auto widgetAction = dynamic_cast<action::WidgetAction*>(action);
            auto widgetHwnd = widgetAction->GetWidget()->GetHWND();
            EnableWindow(widgetHwnd, action->IsEnabled());
            continue;
        }
        else 
        {
            SendMessage(m_hwndToolbar, TB_ENABLEBUTTON, static_cast<int>(action->GetType()), MAKELPARAM(action->IsEnabled(), 0));
            SendMessage(m_hwndToolbar, TB_CHECKBUTTON, static_cast<int>(action->GetType()), MAKELPARAM(action->IsChecked(), 0));
        }
    }
}
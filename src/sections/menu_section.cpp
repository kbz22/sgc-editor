#include "sections/menu_section.hpp"
#include "win32_helpers/create_helpers.hpp"
#include "program/program.hpp"
#include "action/action_description.hpp"
#include "action/popup_menu_action.hpp"
#include <windows.h>
#include <commctrl.h>
#include <algorithm>

sections::MenuSection::MenuSection(program::ProgramContext& programContext)    
{
    using namespace win32_program;

    auto win32context = programContext.mainWindowContext.get();

    auto hwndRebar = win32_helpers::CreateRebar(
        win32context->hMainWindow,
        win32context->hInstance,
        static_cast<types::ctrid_t>(ControlId::MenuRebar)
    );

    auto hwndToolbar = win32_helpers::CreateToolbar(
        hwndRebar,
        win32context->hInstance,
        static_cast<types::ctrid_t>(ControlId::MenuToolbar)
    );

    // Making menu bar buttons
    {
    // Required for TBADDBUTTONS to work correctly
    SendMessage(hwndToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
    
    std::vector<TBBUTTON> tbButtons;
    tbButtons.reserve(static_cast<int>(action::MenuId::Count));

    for (int i=static_cast<int>(action::MenuId::File); i<static_cast<int>(action::MenuId::Count); ++i)
    {
        auto menuItems = programContext.actionManager->GetMenuActions(static_cast<action::MenuId>(i));        

        std::sort(menuItems.begin(), menuItems.end(), [](const action::Action* a, const action::Action* b) {
            return a->GetMenuIndex() < b->GetMenuIndex();
        });

        if(menuItems.empty()) {
            continue;
        }

        auto menuAction = reinterpret_cast<action::PopupMenuAction*>(menuItems[0]);
        auto menuNameId = menuAction->GetNameStringId();

        if(menuNameId == std::nullopt) {
            continue;
        }

        auto &menuNameText = programContext.stringLookup.Get(menuNameId.value());
        
        if(menuNameText == std::nullopt) {
            throw std::runtime_error("PopupMenuAction::BuildMenu: Missing text for menu item.");
        }

        int strIndex = static_cast<int>(
            SendMessage(
                hwndToolbar,
                TB_ADDSTRING,
                0,
                reinterpret_cast<LPARAM>(menuNameText.value().c_str())
            )
        );

        TBBUTTON btn = {};
        btn.iBitmap = I_IMAGENONE;
        btn.idCommand = static_cast<int>(menuAction->GetType());
        btn.fsState = menuAction->IsEnabled() ? TBSTATE_ENABLED : 0;
        btn.fsStyle = BTNS_BUTTON | BTNS_SHOWTEXT;
        btn.iString = strIndex;        

        tbButtons.push_back(btn);

        if(menuAction->IsPopup()) {
            menuItems.erase(menuItems.begin());

            menuAction->SetItems(menuItems);
            menuAction->BuildMenu(programContext);
        }
    }

    SendMessage(hwndToolbar, TB_ADDBUTTONS,
        static_cast<WPARAM>(tbButtons.size()),
        reinterpret_cast<LPARAM>(tbButtons.data())
    );
    
    SendMessage(hwndToolbar, TB_SETBUTTONSIZE, 0, MAKELPARAM(34, 0));
    SendMessage(hwndToolbar, TB_SETPADDING, 0, MAKELPARAM(3, 0));
    SendMessage(hwndToolbar, TB_AUTOSIZE, 0, 0);

    }

    // Setting up the rebar (i think mostly for size)
    SIZE sz = {};
    SendMessage(hwndToolbar, TB_GETMAXSIZE, 0, (LPARAM)&sz);

    REBARBANDINFO rb = { sizeof(rb) };
    rb.fMask = RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE;
    rb.hwndChild = hwndToolbar;
    rb.cxMinChild = sz.cx;
    rb.cyMinChild = sz.cy;
    rb.fStyle = RBBS_CHILDEDGE;

    SendMessage(hwndRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rb);

    SetHwnd(hwndRebar, win32context->hMainWindow);
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

void sections::MenuSection::Refresh(program::ProgramContext& context)
{
    for (int i=static_cast<int>(action::MenuId::File); i<static_cast<int>(action::MenuId::Count); ++i) 
    {
        auto menuActions = context.actionManager->GetMenuActions(static_cast<action::MenuId>(i));

        for(auto* action : menuActions)
        {
            if(action->IsPopup())
            {
                auto popup = reinterpret_cast<action::PopupMenuAction*>(action);
                popup->RefreshMenu();
            }
        }
    }
}
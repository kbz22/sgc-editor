#include "action/popup_menu_action.hpp"
#include "program/program.hpp"
#include <stdexcept>

action::PopupMenuAction::PopupMenuAction()
{
    m_hMenu = CreatePopupMenu();
    m_popupMenuItems = std::vector<Action*>{};
}

void action::PopupMenuAction::BuildMenu(program::ProgramContext &context)
{
    for(auto &item : m_popupMenuItems)
    {
        auto text = context.stringLookup.Get(item->GetNameStringId().value());
        
        if(text == std::nullopt) {
            throw std::runtime_error("PopupMenuAction::BuildMenu: Missing text for menu item.");
        }

        if(item->IsPopup())
        {
            auto popupItem = dynamic_cast<PopupMenuAction*>(item);

            popupItem->BuildMenu(context);

            AppendMenuW(
                m_hMenu,
                MF_POPUP,
                reinterpret_cast<UINT_PTR>(popupItem->GetHMenu()),
                text.value().c_str()
            );
        }
        else
        {
            AppendMenuW(
                m_hMenu,
                MF_STRING,
                static_cast<int>(item->GetType()),
                text.value().c_str()
            );
        }
    }
}

std::optional<std::vector<action::Action*>> action::PopupMenuAction::GetPopupMenuItems() const
{
    if(m_popupMenuItems.empty()) {
        return std::nullopt;
    }

    return m_popupMenuItems;
}

void action::PopupMenuAction::SetItems(std::vector<Action*> items)
{
    m_popupMenuItems.clear();

    for(auto &item : items) {
        m_popupMenuItems.push_back(item);
    }    
}

bool action::PopupMenuAction::IsPopup() const
{
    return true;
}

HMENU action::PopupMenuAction::GetHMenu() const
{
    return m_hMenu;
}

void action::PopupMenuAction::Execute(program::ProgramContext& context)
{
    if(m_menuId < 0) {
        throw std::runtime_error("PopupMenuAction::Execute: m_menuId is not set.");
    }

    RECT rc{};

    SendMessage(
        context.menuSection->GetHwndToolbar(),
        TB_GETRECT,
        m_menuId,
        reinterpret_cast<LPARAM>(&rc));

    MapWindowPoints(
        context.menuSection->GetHwndToolbar(),
        HWND_DESKTOP,
        reinterpret_cast<POINT*>(&rc),
        2);

    TrackPopupMenu(
        m_hMenu,
        TPM_LEFTALIGN | TPM_TOPALIGN,
        rc.left,
        rc.bottom,
        0,
        context.menuSection->GetHwndToolbar(),
        nullptr
    );

    return;
}

void action::PopupMenuAction::RefreshMenu()
{
    for (auto& item : m_popupMenuItems)
    {
        if(item->IsPopup())
        {
            auto popupItem = dynamic_cast<PopupMenuAction*>(item);
            popupItem->RefreshMenu();
        }
        else
        {
            EnableMenuItem(
                m_hMenu,
                static_cast<UINT>(item->GetType()),
                item->IsEnabled()
                    ? MF_ENABLED | MF_BYCOMMAND
                    : MF_GRAYED
            );

            CheckMenuItem(
                m_hMenu,
                static_cast<UINT>(item->GetType()),
                item->IsChecked()
                    ? MF_CHECKED | MF_BYCOMMAND
                    : MF_UNCHECKED
            );
        }
    }
}
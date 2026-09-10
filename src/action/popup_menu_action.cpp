#include "action/popup_menu_action.hpp"
#include "program/program.hpp"
#include <stdexcept>
#include <algorithm>

action::PopupMenuAction::PopupMenuAction()
{
    m_hMenu = CreatePopupMenu();
    m_popupMenuItems = std::vector<Action*>{};
}

void action::PopupMenuAction::BuildMenu(program::ProgramContext &context)
{
    if(m_popupMenuItems.empty()) {
        return;
    }

    auto previousGroupId = m_popupMenuItems[0]->GetGroupId();

    for(auto &item : m_popupMenuItems)
    {
        auto stringLookup = context.GetStringLookup();
        auto text = stringLookup.Get(item->GetNameStringId().value());

        if(item->GetGroupId() != previousGroupId) {
            AppendMenuW(
                m_hMenu,
                MF_SEPARATOR,
                0,
                nullptr
            );

            previousGroupId = item->GetGroupId();
        }
        
        if(text == std::nullopt) {
            throw std::runtime_error("PopupMenuAction::BuildMenu: Missing text for menu item.");
        }

        if(item->IsPopup())
        {
            auto popupItem = dynamic_cast<PopupMenuAction*>(item);

            popupItem->BuildMenu(context);

            MENUITEMINFOW info{};
            info.cbSize = sizeof(info);
            info.fMask = MIIM_ID | MIIM_SUBMENU | MIIM_STRING;
            info.wID = static_cast<UINT>(item->GetType());
            info.hSubMenu = popupItem->GetHMenu();
            info.dwTypeData = text.value().data();

            InsertMenuItemW(
                m_hMenu,
                GetMenuItemCount(m_hMenu),
                TRUE,
                &info
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

    auto menuSection = context.GetSection<sections::MenuSection>();

    RECT rc{};

    SendMessage(
        menuSection->GetHwndToolbar(),
        TB_GETRECT,
        m_menuId,
        reinterpret_cast<LPARAM>(&rc));

    MapWindowPoints(
        menuSection->GetHwndToolbar(),
        HWND_DESKTOP,
        reinterpret_cast<POINT*>(&rc),
        2);

    TrackPopupMenu(
        m_hMenu,
        TPM_LEFTALIGN | TPM_TOPALIGN,
        rc.left,
        rc.bottom,
        0,
        menuSection->GetHwndToolbar(),
        nullptr
    );

    return;
}

void action::PopupMenuAction::RefreshMenu()
{
    for(auto& item : m_popupMenuItems)
    {
        EnableMenuItem(
            m_hMenu,
            static_cast<UINT>(item->GetType()),
            item->IsEnabled()
                ? MF_ENABLED | MF_BYCOMMAND
            : MF_GRAYED
        );
        
        if(item->IsPopup())
        {
            auto popupItem = dynamic_cast<PopupMenuAction*>(item);
            popupItem->RefreshMenu();
        }
        else
        {            
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
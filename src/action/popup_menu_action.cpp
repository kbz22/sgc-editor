#include "action/popup_menu_action.hpp"
#include "program/program.hpp"

action::PopupMenuAction::PopupMenuAction()
{
    m_hMenu = CreatePopupMenu();
    m_popupMenuItems = std::vector<Action*>{};
}

void action::PopupMenuAction::BuildMenu(program::ProgramContext &context)
{
    for(auto &item : m_popupMenuItems)
    {
        if(item->IsPopup()) {
            auto popupItem = dynamic_cast<PopupMenuAction*>(item);
            auto subItems = popupItem->GetPopupMenuItems();

            if(subItems != std::nullopt) {
                popupItem->BuildMenu(context);
            }
        }

        auto text = context.stringLookup.Get(item->GetNameStringId().value()).c_str();

        AppendMenuW(
            m_hMenu,
            MF_STRING,
            static_cast<int>(item->GetType()),
            text
        );
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
    return;
}


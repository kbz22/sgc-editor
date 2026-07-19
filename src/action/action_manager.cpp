#include "action/action_manager.hpp"

void action::ActionManager::Register(std::unique_ptr<Action> action)
{
    m_actionLookup[action->GetType()] = action.get();
    m_actions.push_back(std::move(action));
}

action::Action* action::ActionManager::Find(ActionType actionType)
{
    auto it = m_actionLookup.find(actionType);

    if (it != m_actionLookup.end()) {
        return it->second;
    }

    return nullptr;
}

void action::ActionManager::Execute(ActionType actionType, program::ProgramContext& context)
{
    auto it = m_actionLookup.find(actionType);
    if (it == m_actionLookup.end())
        return;

    Action& action = *it->second;

    if (!action.IsEnabled())
        return;

    action.Execute(context);
}

std::vector<action::Action*> action::ActionManager::GetToolbarActions() const
{
    std::vector<Action*> toolbarActions;

    for (const auto& action : m_actions) {
        Action* ptr = action.get();
        if (ptr->IsToolbarItem()) {
            toolbarActions.push_back(ptr);
        }
    }

    return toolbarActions;
}

std::vector<action::Action*> action::ActionManager::GetMenuActions(MenuId menu) const
{
    std::vector<Action*> menuActions;

    for (const auto& action : m_actions) {
        Action* ptr = action.get();
        if (ptr->GetMenuId() == menu) {
            menuActions.push_back(ptr);
        }
    }

    return menuActions;
}
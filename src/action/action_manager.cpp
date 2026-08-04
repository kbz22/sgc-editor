#include "action/action_manager.hpp"

action::Action* action::ActionManager::Register(std::unique_ptr<Action> action)
{    
    m_actions.push_back(std::move(action));
    return m_actions.back().get();
}

action::Action* action::ActionManager::Find(ActionType actionType)
{
    for (const auto& action : m_actions) {
        if (action->GetType() == actionType) {
            return action.get();
        }
    }

    return nullptr;
}

void action::ActionManager::Execute(ActionType actionType, program::ProgramContext& context)
{
    Action* action = Find(actionType);
    if (action == nullptr)
        return;

    Action& actionRef = *action;

    if (!actionRef.IsEnabled())
        return;

    actionRef.Execute(context);
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

void action::ActionManager::ActionSetEnabled(ActionType actionType, bool enabled)
{
    for(auto &action : m_actions) {
        if(action->GetType() == actionType) {
            action->SetEnabled(enabled);
            break;
        }
    }
}

void action::ActionManager::ActionSetEnabled(const std::vector<ActionType>& actionTypes, bool enabled)
{
    for(auto &actionType : actionTypes) {
        ActionSetEnabled(actionType, enabled);
    }
}

void action::ActionManager::ActionSetChecked(ActionType actionType, bool checked)
{
    for(auto &action : m_actions) {
        if(action->GetType() == actionType) {
            action->SetChecked(checked);
        }
    }
}

void action::ActionManager::ActionSetChecked(const std::vector<ActionType>& actionTypes, bool checked)
{
    for(auto &actionType : actionTypes) {
        ActionSetChecked(actionType, checked);
    }
}
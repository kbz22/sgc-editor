#pragma once

#include "action/action.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

namespace action {

    class ActionManager
    {
        private:            
            std::vector<std::unique_ptr<Action>> m_actions;

        public:
            void Register(std::unique_ptr<Action> action);
            void Execute(ActionType actionType, program::ProgramContext& context);
            Action* Find(ActionType actionType);

            std::vector<Action*> GetToolbarActions() const;
            std::vector<Action*> GetMenuActions(MenuId menu) const;

            void ActionSetEnabled(ActionType actionType, bool enabled);
            void ActionSetEnabled(const std::vector<ActionType>& actionTypes, bool enabled);
            void ActionSetChecked(ActionType actionType, bool checked);

    };

}
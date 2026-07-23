#pragma once

#include "action/action_types.hpp"
#include "action/action_description.hpp"
#include "win32_program/windows_controls.hpp"
#include <string>
#include <optional>
#include <vector>
#include <memory>

namespace program {
    struct ProgramContext;
}

namespace action {

    class Action 
    {
        protected:
            ActionType m_actionType = ActionType::Default;
            ActionDescription m_actionDescription{};
            bool m_enabled = false;            
            bool m_checked = false;            

            win32_program::CommandId m_commandId; //! tmp - to be fully replaced by ActionType

        public:
            virtual ~Action() = default;

            ActionType GetType() const;

            bool IsEnabled() const;         
            bool IsChecked() const;
            bool IsCheckGroupItem() const;
            bool IsToolbarItem() const;
            virtual bool IsPopup() const;

            void SetEnabled(bool enabled);            
            void SetChecked(bool checked);

            int GetToolbarImageIndex() const;
            int GetToolbarOrder() const;
            int GetMenuIndex() const;
            GroupId GetGroupId() const;
            MenuId GetMenuId() const;            
            std::optional<locale::StringId> GetTooltipStringId() const;
            std::optional<locale::StringId> GetNameStringId() const;
            virtual std::optional<std::vector<Action*>> GetPopupMenuItems() const;

            win32_program::CommandId GetCommandId() const;

            virtual void Execute(program::ProgramContext& context) = 0;
    };

}
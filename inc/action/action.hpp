#pragma once

#include "action/action_types.hpp"
#include "win32_program/windows_controls.hpp"
#include "program/program.hpp"
#include <string>

namespace action {

    class Action 
    {
        private:
            ActionType m_actionType = ActionType::Default;
            bool m_enabled = false;            
            bool m_grouped = false;
            bool m_checked = false;

            win32_program::CommandId m_commandId; //! tmp - to be fully replaced by ActionType

        public:
            ~Action() = default;

            ActionType GetType() const;
            // std::wstring GetName(program::ProgramContext const& context) const; //!
            std::wstring GetTooltip(program::ProgramContext const& context) const;

            bool IsEnabled() const;
            bool IsSeperator() const;
            bool IsGrouped() const;
            bool IsChecked() const;

            void SetEnabled(bool enabled);            
            void SetChecked(bool checked);

            win32_program::CommandId GetCommandId() const;
    };

}
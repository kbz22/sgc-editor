#pragma once

#include "action/action.hpp"
#include "editor_tools/editor_tool_manager.hpp"

namespace action {

    class ChangeSelectionModeAction : public Action
    {
        private:
            editor_tools::SelectionMode m_selectionMode;

        public:
            ChangeSelectionModeAction(editor_tools::SelectionMode selectionMode);
            ~ChangeSelectionModeAction() = default;

            void Execute(program::ProgramContext& programContext) override;
    };

}
#pragma once

#include "action/action.hpp"
#include "program/editor_mode.hpp"

namespace action {

    class ChangeGridModeAction : public Action
    {
        private:
            program::EditorGridMode m_newGridMode;

        public:
            ChangeGridModeAction(program::EditorGridMode newGridMode);

            void Execute(program::ProgramContext& context) override;
    };

}
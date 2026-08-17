#pragma once

#include "action/action.hpp"

namespace action {

    class SelectionCutAction : public Action {

        public:
            SelectionCutAction();
            ~SelectionCutAction() = default;

            void Execute(program::ProgramContext &context) override;

    };

}
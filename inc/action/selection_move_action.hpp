#pragma once

#include "action/action.hpp"

namespace action {

    class SelectionMoveAction : public Action
    {
        public:
            SelectionMoveAction();
            ~SelectionMoveAction() = default;

            void Execute(program::ProgramContext &context) override;
    };

}
#pragma once

#include "action/action.hpp"

namespace action {

    class SelectionClearAction : public Action
    {
        public:
            SelectionClearAction();
            ~SelectionClearAction() = default;

            void Execute(program::ProgramContext& context) override;
    };    

}
#pragma once

#include "action/action.hpp"

namespace action {

    class UndoAction : public Action
    {
        public:
            UndoAction();

            void Execute(program::ProgramContext& context) override;
    };

}
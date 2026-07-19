#pragma once

#include "action/action.hpp"

namespace action {

    class RedoAction : public Action
    {
        public:
            RedoAction();

            void Execute(program::ProgramContext& context) override;
    };

}
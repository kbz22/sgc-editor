#pragma once

#include "action/action.hpp"

namespace action {

    class ZoomResetAction : public Action
    {
        public:
            ZoomResetAction();

            void Execute(program::ProgramContext& context) override;
    };

}
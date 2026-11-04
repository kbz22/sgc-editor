#pragma once

#include "action/action.hpp"

namespace action {

    class SymbolDrawingEnableAction : public Action
    {
        public:
            SymbolDrawingEnableAction();

            void Execute(program::ProgramContext& context) override;
    };

}
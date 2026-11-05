#pragma once

#include "action/action.hpp"

namespace action
{
    class SymbolChangeGroupAction : public Action
    {
        public:
            SymbolChangeGroupAction();

            void Execute(program::ProgramContext &context) override;
    };
}
#pragma once

#include "action/action.hpp"

namespace action
{
    class SymbolChangeAction : public Action
    {
        public:
            SymbolChangeAction();

            void Execute(program::ProgramContext& context) override;
    };
}
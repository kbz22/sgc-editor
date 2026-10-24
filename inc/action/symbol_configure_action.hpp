#pragma once

#include "action/action.hpp"

namespace action
{
    class SymbolConfigureAction : public Action
    {
        public:
            SymbolConfigureAction();

            void Execute(program::ProgramContext& context) override;
    };
}
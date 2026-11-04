#pragma once

#include "action/action.hpp"

namespace action {

    class SymbolAutoCycleAction : public Action
    {
        public:
            SymbolAutoCycleAction();

            void Execute(program::ProgramContext& context) override;
    };

}
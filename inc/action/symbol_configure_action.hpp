#pragma once

#include "action/action.hpp"

namespace action
{
    class SymbolConfigureAction : public Action
    {
        private:
            static bool m_registered;

        public:
            SymbolConfigureAction();

            void Execute(program::ProgramContext& context) override;
    };
}
#pragma once

#include "action/action.hpp"

namespace action
{
    class SaveAsAction : public Action
    {
        public:
            SaveAsAction();

            virtual void Execute(program::ProgramContext& programContext) override;
    };
}
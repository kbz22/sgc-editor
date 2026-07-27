#pragma once

#include "action/action.hpp"

namespace action
{
    class SaveFileAction : public Action
    {
        public:
            SaveFileAction();

            virtual void Execute(program::ProgramContext& programContext) override;
    };
}
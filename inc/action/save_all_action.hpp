#pragma once

#include "action/action.hpp"

namespace action
{
    class SaveAllAction : public Action
    {
        public:
            SaveAllAction();

            virtual void Execute(program::ProgramContext& programContext) override;
    };
}
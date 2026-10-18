#pragma once

#include "action/action.hpp"

namespace action 
{  

    class HelpRepositoryAction : public Action
    {
        public:
            HelpRepositoryAction();
            
            bool IsShortcutable() const override;
            void Execute(program::ProgramContext& programContext) override;
    };

}
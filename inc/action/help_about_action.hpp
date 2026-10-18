#pragma once

#include "action/action.hpp"

namespace action {

    class HelpAboutAction : public Action
    {
        public:
            HelpAboutAction();
            
            bool IsShortcutable() const override;
            void Execute(program::ProgramContext& programContext) override;
    };

}
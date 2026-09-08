#pragma once

#include "action/action.hpp"

namespace action {

    class SettingsAction : public Action
    {
        public:
            SettingsAction();

            void Execute(program::ProgramContext& context) override;
    };

}
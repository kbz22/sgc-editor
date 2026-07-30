#pragma once

#include "action/action.hpp"

namespace action {

    class OpenFileAction : public Action
    {
        public:
            OpenFileAction();

            void Execute(program::ProgramContext& context) override;
    };

}
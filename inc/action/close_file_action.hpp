#pragma once

#include "action/action.hpp"

namespace action {

    class CloseFileAction : public Action
    {
        public:
            CloseFileAction();

            void Execute(program::ProgramContext& context) override;
    };

}
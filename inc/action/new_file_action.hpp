#pragma once

#include "action/action.hpp"

namespace action {

    class NewFileAction : public Action
    {
        public:
            NewFileAction();

            void Execute(program::ProgramContext& context) override;
    };

}
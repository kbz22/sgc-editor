#pragma once

#include "action/action.hpp"

namespace action {

    class SelectionCopyAction : public Action {
        
        public:
            SelectionCopyAction();
            ~SelectionCopyAction() = default;

            void Execute(program::ProgramContext &context) override;

    };

}
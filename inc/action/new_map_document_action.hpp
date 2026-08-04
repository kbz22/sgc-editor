#pragma once

#include "action/action.hpp"

namespace action {

    class NewMapDocumentAction : public Action
    {
        public:
            NewMapDocumentAction();

            void Execute(program::ProgramContext& context) override;
    };

}
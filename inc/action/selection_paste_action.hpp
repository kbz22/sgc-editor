#pragma once

#include "action/action.hpp"

namespace action {

    class SelectionPasteAction : public Action {

        public:
            SelectionPasteAction();
            ~SelectionPasteAction() = default;

            void Execute(program::ProgramContext &context) override;

    };

}
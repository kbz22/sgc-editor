#pragma once

#include "action/action.hpp"

namespace action {

    class NewTilesetDocumentAction : public Action
    {
        public:
            NewTilesetDocumentAction();

            void Execute(program::ProgramContext& context) override;
    };

}
#pragma once

#include "action/action.hpp"

namespace action 
{
    class TilePickerToolAction : public Action
    {
        public:
            TilePickerToolAction();

            void Execute(program::ProgramContext& context) override;
    };
}
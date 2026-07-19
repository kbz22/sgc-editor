#pragma once

#include "action/action.hpp"

namespace action {

    class LayerRemoveAction : public Action
    {
        public:
            LayerRemoveAction();
            virtual void Execute(program::ProgramContext& context) override;
    };

}
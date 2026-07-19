#pragma once

#include "action/action.hpp"

namespace action {

    class LayerAddAction : public Action
    {
        public:
            LayerAddAction();
            virtual void Execute(program::ProgramContext& context) override;
    };

}
#pragma once

#include "action/action.hpp"

namespace action {

    class LayerMoveAction : public Action
    {
        private:
            int m_moveCount = 0;
            
        public:
            LayerMoveAction(int moveCount);
            virtual void Execute(program::ProgramContext& context) override;
    };

}
#pragma once

#include "action/action.hpp"

namespace action {

    class ZoomAction : public Action
    {
        private:
            float m_zoomFactor;
            
        public:
            ZoomAction(float zoomFactor);

            void Execute(program::ProgramContext& context) override;
    };

}
#pragma once

#include "action/action.hpp"

namespace action {

    enum class ChangeActiveLayerDirection
    {
        Up,
        Down,
        Top,
        Bottom
    };

    class ChangeActiveLayerAction : public Action
    {
        private:
            ChangeActiveLayerDirection m_direction;

        public:
            ChangeActiveLayerAction(ChangeActiveLayerDirection direction);

            void Execute(program::ProgramContext& context) override;
    };

}
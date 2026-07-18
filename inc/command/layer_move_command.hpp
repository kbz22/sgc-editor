#pragma once

#include "command/icommand.hpp"

namespace command {

    class LayerMoveCommand : public ICommand
    {
        private:
            int m_movement;

        public:
            LayerMoveCommand(int movement) : m_movement{movement} {}
            virtual ~LayerMoveCommand() = default;

            void Execute() override;
            void Undo() override;
    };

}
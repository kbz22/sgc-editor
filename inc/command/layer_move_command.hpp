#pragma once

#include "command/icommand.hpp"

namespace command {

    class LayerMoveCommand : public ICommand
    {
        private:
            int m_movement;

        public:
            LayerMoveCommand(int movement) : m_movement{movement} {}
            LayerMoveCommand(const LayerMoveCommand& other);
            virtual ~LayerMoveCommand() = default;

            void Execute() override;
            void Commit() override;
            void Undo() override;
    };

}
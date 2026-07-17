#pragma once

#include <vector>
#include <memory>
#include "command/icommand.hpp"

namespace command {

    class CommandManager {

        private:
            std::vector<std::unique_ptr<ICommand>> m_undoStack{};
            std::vector<std::unique_ptr<ICommand>> m_redoStack{};

        public:
            CommandManager() = default;
            ~CommandManager() = default;

            void Execute(std::unique_ptr<ICommand> command);
            void Undo();
            void Redo();

            bool CanUndo() const;
            bool CanRedo() const;

            void Reset();


    };

}
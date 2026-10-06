#pragma once

#include "command/icommand.hpp"
#include <vector>
#include <memory>

namespace command {

    class BulkCommand : public ICommand
    {
        private:
            std::vector<std::unique_ptr<ICommand>> m_commands;

        public:
            BulkCommand() = default;
            BulkCommand(const BulkCommand& other);

            void AddCommand(std::unique_ptr<ICommand> command);
            bool Empty() const;

            void Execute() override;
            void Commit() override;
            void Undo() override;
    };

}
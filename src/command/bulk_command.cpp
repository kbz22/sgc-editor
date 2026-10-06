#include "command/bulk_command.hpp"

void command::BulkCommand::AddCommand(std::unique_ptr<ICommand> command)
{
    m_commands.push_back(std::move(command));
}

void command::BulkCommand::Execute()
{
    for(auto& command : m_commands) {
        command->Execute();
    }
}

void command::BulkCommand::Commit()
{
    for(auto& command : m_commands) {
        command->Commit();
    }
}

void command::BulkCommand::Undo()
{
    for(auto it = m_commands.rbegin(); it != m_commands.rend(); ++it) {
        (*it)->Undo();
    }
}

bool command::BulkCommand::Empty() const
{
    return m_commands.empty();
}
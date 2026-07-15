#include "locale/command_manager.hpp"
#include <stdexcept>

const locale::CommandInfo& locale::CommandManager::Get(win32_program::CommandId id) const
{
    for (const auto& command : m_commands)
    {
        if (command.id == id)
        {
            return command;
        }
    }

    throw std::runtime_error("Command not found");
}
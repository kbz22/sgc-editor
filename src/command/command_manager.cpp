#include "command/command_manager.hpp"

void command::CommandManager::Execute(std::unique_ptr<ICommand> command)
{
    command->Execute();
    m_undoStack.push_back(std::move(command));
    m_redoStack.clear();
}

void command::CommandManager::Commit(std::unique_ptr<ICommand> command)
{
    m_undoStack.push_back(std::move(command));
    m_redoStack.clear();
}

void command::CommandManager::Undo()
{
    if (!CanUndo()) {
        return;
    }

    auto command = std::move(m_undoStack.back());
    m_undoStack.pop_back();
    command->Undo();
    m_redoStack.push_back(std::move(command));
}

void command::CommandManager::Redo()
{
    if (!CanRedo()) {
        return;
    }

    auto command = std::move(m_redoStack.back());
    m_redoStack.pop_back();
    command->Execute();
    m_undoStack.push_back(std::move(command));
}

bool command::CommandManager::CanUndo() const
{
    return !m_undoStack.empty();
}

bool command::CommandManager::CanRedo() const
{
    return !m_redoStack.empty();
}

void command::CommandManager::Reset()
{
    m_undoStack.clear();
    m_redoStack.clear();
}
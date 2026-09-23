#include "program/shortcut_settings_manager.hpp"
#include "program/program.hpp"
#include "locale/shortcut_to_string.hpp"

void program::ShortcutSettingsManager::AddShortcutEntry(const ShortcutEntry& entry)
{
    m_shortcutEntries[entry.index] = entry;
}

void program::ShortcutSettingsManager::RemoveShortcutEntry(int index)
{
    m_shortcutEntries.erase(index);
}

program::ShortcutEntry* program::ShortcutSettingsManager::GetShortcutEntry(int index)
{
    auto it = m_shortcutEntries.find(index);
    if (it != m_shortcutEntries.end())
    {
        return &it->second;
    }
    return nullptr;
}

void program::ShortcutSettingsManager::SetEditedShortcutEntry(const ShortcutEntry& entry)
{
    m_editedShortcut = entry;
}

void program::ShortcutSettingsManager::SetEditedShortcut(const win32_program::Shortcut& shortcut, program::ProgramContext& programContext)
{
    m_editedShortcut.shortcut = shortcut;
    m_editedShortcut.shortcutString = locale::ShortcutToString(shortcut, programContext);
}

bool program::ShortcutSettingsManager::IsEditing()
{
    return m_editedShortcut.actionType != action::ActionType::Default;
}

void program::ShortcutSettingsManager::CommitEdit(win32_program::ShortcutManager& shortcutManager)
{
    if (m_editedShortcut.actionType == action::ActionType::Default)
        return;

    auto currentShortcut = m_shortcutEntries[m_editedShortcut.index];

    shortcutManager.ReplaceShortcut(
        m_editedShortcut.actionType,
        currentShortcut.shortcut,
        m_editedShortcut.shortcut
    );

    m_shortcutEntries[m_editedShortcut.index] = m_editedShortcut;

    CancelEdit();
}

void program::ShortcutSettingsManager::CancelEdit()
{
    m_editedShortcut = ShortcutEntry{};
}

program::ShortcutEntry* program::ShortcutSettingsManager::GetEditedShortcut()
{
    return &m_editedShortcut;
}
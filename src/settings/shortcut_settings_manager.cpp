#include "settings/shortcut_settings_manager.hpp"
#include "program/program.hpp"
#include "locale/shortcut_to_string.hpp"

void settings::ShortcutSettingsManager::AddShortcutEntry(const ShortcutEntry& entry)
{
    m_shortcutEntries[entry.index] = entry;
}

void settings::ShortcutSettingsManager::RemoveShortcutEntry(int index)
{
    m_shortcutEntries.erase(index);
}

settings::ShortcutEntry* settings::ShortcutSettingsManager::GetShortcutEntry(int index)
{
    auto it = m_shortcutEntries.find(index);
    if (it != m_shortcutEntries.end())
    {
        return &it->second;
    }
    return nullptr;
}

void settings::ShortcutSettingsManager::SetEditedShortcutEntry(const ShortcutEntry& entry)
{
    m_editedShortcut = entry;
}

void settings::ShortcutSettingsManager::SetEditedShortcut(const win32_program::Shortcut& shortcut, program::ProgramContext& programContext)
{
    m_editedShortcut.shortcut = shortcut;
    m_editedShortcut.shortcutString = locale::ShortcutToString(shortcut, programContext);
}

bool settings::ShortcutSettingsManager::IsEditing()
{
    return m_editedShortcut.actionType != action::ActionType::Default;
}

void settings::ShortcutSettingsManager::CommitEdit(win32_program::ShortcutManager& shortcutManager)
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

void settings::ShortcutSettingsManager::CancelEdit()
{
    m_editedShortcut = ShortcutEntry{};
}

settings::ShortcutEntry* settings::ShortcutSettingsManager::GetEditedShortcut()
{
    return &m_editedShortcut;
}
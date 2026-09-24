#include "settings/shortcuts_setting.hpp"

settings::ShortcutsSetting::ShortcutsSetting(win32_program::ShortcutManager& shortcutManager) :
    m_shortcutSettingsManager{std::make_unique<ShortcutSettingsManager>()},
    m_shortcutManager{shortcutManager}
{}

std::vector<uint8_t> settings::ShortcutsSetting::GetBytes() const
{
    return {};
}

void settings::ShortcutsSetting::LoadFromBytes(const std::vector<uint8_t>& bytes)
{
}

settings::ShortcutSettingsManager& settings::ShortcutsSetting::GetShortcutSettingsManager() const
{
    return *m_shortcutSettingsManager;
}

win32_program::ShortcutManager& settings::ShortcutsSetting::GetShortcutManager() const
{
    return m_shortcutManager;
}

void settings::ShortcutsSetting::Commit()
{
    m_shortcutSettingsManager->CommitEdit(m_shortcutManager);
}
#include "settings/settings_manager.hpp"
#include "settings/settings.hpp"
#include <stdexcept>

void settings::SettingsManager::SetCategory(SettingCategory category)
{
    if(category == SettingCategory::Count){
        throw std::invalid_argument("Cannot set category to Count");
    }

    m_currentCategory = category;
}

void settings::SettingsManager::SetCategoryWindow(SettingCategory category, HWND hwnd)
{
    if(category == SettingCategory::Count){
        throw std::invalid_argument("Cannot set category window for Count");
    }

    m_categoryWindows[category] = hwnd;
}

settings::SettingCategory settings::SettingsManager::GetCurrentCategory() const
{
    return m_currentCategory;
}

HWND settings::SettingsManager::GetCategoryWindow(SettingCategory category) const
{
    auto it = m_categoryWindows.find(category);
    return it != m_categoryWindows.end() ? it->second : nullptr;
}

HWND settings::SettingsManager::GetCurrentCategoryWindow() const
{
    return GetCategoryWindow(m_currentCategory);
}

void settings::SettingsManager::CommitChanges()
{
    for(auto &pair : m_settings){
        if(pair.second){
            pair.second->Commit();
        }
    }
}

template<>
void settings::SettingsManager::RegisterSetting<settings::ShortcutsSetting>(unsigned key, std::unique_ptr<settings::ShortcutsSetting> setting)
{
    SettingKey settingKey{SettingCategory::Shortcuts, key};
    m_settings[settingKey] = std::move(setting);
}

template<>
settings::ShortcutsSetting* settings::SettingsManager::GetSetting<settings::ShortcutsSetting>(unsigned key) const
{
    SettingKey settingKey{SettingCategory::Shortcuts, key};
    auto it = m_settings.find(settingKey);
    return it != m_settings.end() ? dynamic_cast<settings::ShortcutsSetting*>(it->second.get()) : nullptr;
}
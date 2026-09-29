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
            m_preferences.Set<ISetting*>(pair.second.get());
        }
    }
    m_preferences.Save();
}

bool settings::SettingsManager::LoadValuesFromPreferences()
{
    try
    {
        m_preferences.Load();
        for(auto &pair : m_settings)
        {
            if(pair.second){
                m_preferences.Get(pair.second.get());
            }
        }
    }
    catch(const std::exception&)
    {
        return false;
    }
    
    return true;
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

template<>
void settings::SettingsManager::RegisterSetting<settings::AutoRestoreFilesSetting>(unsigned key, std::unique_ptr<settings::AutoRestoreFilesSetting> setting)
{
    SettingKey settingKey{SettingCategory::General, key};
    m_settings[settingKey] = std::move(setting);
}

template<>
settings::AutoRestoreFilesSetting* settings::SettingsManager::GetSetting<settings::AutoRestoreFilesSetting>(unsigned key) const
{
    SettingKey settingKey{SettingCategory::General, key};
    auto it = m_settings.find(settingKey);
    return it != m_settings.end() ? dynamic_cast<settings::AutoRestoreFilesSetting*>(it->second.get()) : nullptr;
}

template<>
void settings::SettingsManager::RegisterSetting<settings::DefaultOpenFiletypeSetting>(unsigned key, std::unique_ptr<settings::DefaultOpenFiletypeSetting> setting)
{
    SettingKey settingKey{SettingCategory::General, key};
    m_settings[settingKey] = std::move(setting);
}

template<>
settings::DefaultOpenFiletypeSetting* settings::SettingsManager::GetSetting<settings::DefaultOpenFiletypeSetting>(unsigned key) const
{
    SettingKey settingKey{SettingCategory::General, key};
    auto it = m_settings.find(settingKey);
    return it != m_settings.end() ? dynamic_cast<settings::DefaultOpenFiletypeSetting*>(it->second.get()) : nullptr;
}

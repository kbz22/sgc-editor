#include "settings/settings_manager.hpp"
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
#include "win32_program/settings_manager.hpp"
#include <stdexcept>

void win32_program::SettingsManager::SetCategory(SettingCategory category)
{
    if(category == SettingCategory::Count){
        throw std::invalid_argument("Cannot set category to Count");
    }

    m_currentCategory = category;
}

void win32_program::SettingsManager::SetCategoryWindow(SettingCategory category, HWND hwnd)
{
    if(category == SettingCategory::Count){
        throw std::invalid_argument("Cannot set category window for Count");
    }

    m_categoryWindows[category] = hwnd;
}

win32_program::SettingCategory win32_program::SettingsManager::GetCurrentCategory() const
{
    return m_currentCategory;
}

HWND win32_program::SettingsManager::GetCategoryWindow(SettingCategory category) const
{
    auto it = m_categoryWindows.find(category);
    return it != m_categoryWindows.end() ? it->second : nullptr;
}

HWND win32_program::SettingsManager::GetCurrentCategoryWindow() const
{
    return GetCategoryWindow(m_currentCategory);
}
#pragma once

#include "settings/isetting.hpp"

#include <unordered_map>
#include <vector>
#include <memory>
#include <windows.h>

namespace settings {    

    struct SettingKey
    {
        SettingCategory category;
        unsigned key;

        bool operator==(const SettingKey& other) const
        {
            return category == other.category && key == other.key;
        }
    };
}

namespace std {
    template <>
    struct hash<settings::SettingKey>
    {
        std::size_t operator()(const settings::SettingKey& k) const
        {
            return (static_cast<std::size_t>(k.category) << 32) ^ static_cast<std::size_t>(k.key);
        }
    };
}

namespace settings {
    
    class SettingsManager
    {
        private:
            std::unordered_map<SettingKey, std::unique_ptr<settings::ISetting>> m_settings{};
            std::unordered_map<SettingCategory, HWND> m_categoryWindows{};
            SettingCategory m_currentCategory = SettingCategory::General;

        public:
            SettingsManager() = default;
            ~SettingsManager() = default;
            
            template<typename T>
            void RegisterSetting(unsigned key, std::unique_ptr<T> setting);

            void SetCategory(SettingCategory category);
            void SetCategoryWindow(SettingCategory category, HWND hwnd);

            template<typename T>
            T* GetSetting(unsigned key) const;
            
            SettingCategory GetCurrentCategory() const;
            HWND GetCategoryWindow(SettingCategory category) const;
            HWND GetCurrentCategoryWindow() const;

            void CommitChanges();
    };

}
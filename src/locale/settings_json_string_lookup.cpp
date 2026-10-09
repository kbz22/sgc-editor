#include "locale/settings_json_string_lookup.hpp"

locale::SettingsJsonStringLookup::SettingsJsonStringLookup()
{
    m_settingsStrings = std::unordered_map<settings::Key, std::string> {
        {settings::Key::Default, m_valueMissing},
        {settings::Key::AutoRestoreFilesSetting, "auto-restore-files"},
        {settings::Key::DefaultOpenFiletypeSetting, "default-open-filetype"},
        {settings::Key::PointerBehaviourSetting, "pointer-behaviour"}
    };
}

std::string locale::SettingsJsonStringLookup::Get(settings::Key key)
{
    auto keyIt = m_settingsStrings.find(key);
    if(keyIt != m_settingsStrings.end()){
        return keyIt->second;
    }
    return m_valueMissing;
}
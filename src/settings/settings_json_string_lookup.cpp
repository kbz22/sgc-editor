#include "settings/settings_json_string_lookup.hpp"

settings::SettingsJsonStringLookup::SettingsJsonStringLookup()
{
    m_settingsStrings = std::unordered_map<Key, std::string> {
        {Key::Default, m_valueMissing},
        {Key::AutoRestoreFilesSetting, "auto-restore-files"},
        {Key::DefaultOpenFiletypeSetting, "default-open-filetype"}
    };
}

std::string settings::SettingsJsonStringLookup::Get(Key key)
{
    auto keyIt = m_settingsStrings.find(key);
    if(keyIt != m_settingsStrings.end()){
        return keyIt->second;
    }
    return m_valueMissing;
}
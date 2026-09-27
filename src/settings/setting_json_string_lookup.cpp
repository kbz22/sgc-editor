#include "settings/settings_json_string_lookup.hpp"

settings::SettingsJsonStringLookup::SettingsJsonStringLookup()
{
    m_strings = std::unordered_map<Key, std::wstring> {
        {Key::Default, m_valueMissing},
        {Key::AutoRestoreFilesSetting, L"auto-restore-files"},
        {Key::DefaultOpenFiletypeSetting, L"default-open-filetype"}
    };
}

std::wstring settings::SettingsJsonStringLookup::Get(Key key)
{
    auto keyIt = m_strings.find(key);
    if(keyIt != m_strings.end()){
        return keyIt->second;
    }
    return m_valueMissing;
}
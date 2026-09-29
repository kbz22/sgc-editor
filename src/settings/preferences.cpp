#include "settings/preferences.hpp"
#include "win32_helpers/file_helpers.hpp"
#include "settings/settings.hpp"
#include <fstream>

void settings::Preferences::Load()
{
    std::filesystem::path path = win32_helpers::GetPreferencesDirectory();
    std::ifstream file(path);

    if (!file)
    {
        throw std::runtime_error("Failed to open preferences file for reading");
    }
    
    file >> m_json;
}

void settings::Preferences::Save()
{
    std::filesystem::path path = win32_helpers::GetPreferencesDirectory();
    path /= m_preferencesFileName;
    std::ofstream file(path);

    if (!file)
    {
        throw std::runtime_error("Failed to open preferences file for writing");
    }

    file << m_json.dump(4);
}

template<>
void settings::Preferences::Set<settings::AutoRestoreFilesSetting>(settings::AutoRestoreFilesSetting const& setting)
{
    auto settingName = m_stringLookup.Get(settings::Key::AutoRestoreFilesSetting);
    m_json[settingName] = setting.GetValue();
}

template<>
settings::AutoRestoreFilesSetting settings::Preferences::Get<settings::AutoRestoreFilesSetting>()
{
    AutoRestoreFilesSetting setting{};
    auto settingName = m_stringLookup.Get(settings::Key::AutoRestoreFilesSetting);

    if (m_json.contains(settingName))
    {
        setting.SetValue(m_json.at(settingName).get<bool>());
    }

    return setting;
}

template<>
void settings::Preferences::Set<settings::ISetting*>(settings::ISetting* const &settingIterface)
{
    switch(settingIterface->GetKey())
    {
        case Key::AutoRestoreFilesSetting:
        {
            auto autoRestorePtr = dynamic_cast<settings::AutoRestoreFilesSetting*>(settingIterface);
            Set(*autoRestorePtr);
        }

        default:
        {
            return;
        }
    }
}
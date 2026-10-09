#include "settings/preferences.hpp"
#include "win32_helpers/file_helpers.hpp"
#include "settings/settings.hpp"
#include <fstream>

void settings::Preferences::Load()
{
    std::filesystem::path path = win32_helpers::GetPreferencesDirectory();
    path /= m_preferencesFileName;
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
void settings::Preferences::Get<settings::AutoRestoreFilesSetting>(settings::AutoRestoreFilesSetting &setting)
{
    auto settingName = m_stringLookup.Get(settings::Key::AutoRestoreFilesSetting);

    if (m_json.contains(settingName))
    {
        setting.SetValue(m_json.at(settingName).get<bool>());
        setting.Commit();
    }
}

template<>
void settings::Preferences::Set<settings::DefaultOpenFiletypeSetting>(settings::DefaultOpenFiletypeSetting const &setting)
{
    auto name = m_stringLookup.Get(settings::Key::DefaultOpenFiletypeSetting);
    m_json[name] = setting.GetValue();
}

template<>
void settings::Preferences::Get<settings::DefaultOpenFiletypeSetting>(settings::DefaultOpenFiletypeSetting &setting)
{
    auto name = m_stringLookup.Get(settings::Key::DefaultOpenFiletypeSetting);

    if(m_json.contains(name))
    {
        setting.SetValue(m_json.at(name).get<file::FileType>());
        setting.Commit();
    }
}

template<>
void settings::Preferences::Set<settings::PointerBehaviourSetting>(settings::PointerBehaviourSetting const &setting)
{
    auto name = m_stringLookup.Get(settings::Key::PointerBehaviourSetting);
    m_json[name] = setting.GetBehaviours();
}

template<>
void settings::Preferences::Get<settings::PointerBehaviourSetting>(settings::PointerBehaviourSetting &setting)
{
    auto name = m_stringLookup.Get(settings::Key::PointerBehaviourSetting);

    if(m_json.contains(name))
    {
        setting.SetBehaviours(m_json.at(name).get<settings::PointerBehaviourMap>());
        setting.Commit();
    }
}

template<>
void settings::Preferences::Set<settings::ShortcutsSetting>(settings::ShortcutsSetting const &setting)
{
    // do nothing
}

template<>
void settings::Preferences::Get<settings::ShortcutsSetting>(settings::ShortcutsSetting &setting)
{
    // also do nothing
}

template<>
void settings::Preferences::Set<settings::ISetting*>(settings::ISetting* const &settingInterface)
{
    switch(settingInterface->GetKey())
    {
        case Key::AutoRestoreFilesSetting:
        {
            auto autoRestorePtr = dynamic_cast<settings::AutoRestoreFilesSetting*>(settingInterface);
            Set(*autoRestorePtr);
            break;
        }

        case Key::DefaultOpenFiletypeSetting:
        {
            Set(*dynamic_cast<settings::DefaultOpenFiletypeSetting*>(settingInterface));
            break;
        }

        case Key::PointerBehaviourSetting:
        {
            Set(*dynamic_cast<settings::PointerBehaviourSetting*>(settingInterface));
            break;
        }

        default:
        {
            return;
        }
    }
}

void settings::Preferences::Get(settings::ISetting* settingInterface)
{
    switch(settingInterface->GetKey())
    {
        case Key::AutoRestoreFilesSetting:
        {
            Get(*dynamic_cast<settings::AutoRestoreFilesSetting*>(settingInterface));
            break;
        }

        case Key::DefaultOpenFiletypeSetting:
        {
            Get(*dynamic_cast<settings::DefaultOpenFiletypeSetting*>(settingInterface));
            break;
        }

        case Key::PointerBehaviourSetting:
        {
            Get(*dynamic_cast<settings::PointerBehaviourSetting*>(settingInterface));
            break;
        }

        default:
        {
            return;
        }
    }
}
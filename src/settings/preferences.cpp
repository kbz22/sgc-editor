#include "settings/preferences.hpp"
#include "win32_helpers/file_helpers.hpp"
#include "settings/settings.hpp"
#include "program/except.hpp"
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
    std::unordered_map<std::string, std::string> pointerBehavioursStrings;
    auto pointerBehaviours = setting.GetBehaviours();

    for(auto &[type, behaviour] : pointerBehaviours)
    {
        auto typeName = m_stringLookup.Get(type);
        auto behaviourName = m_stringLookup.Get(behaviour);
        pointerBehavioursStrings[typeName] = behaviourName;
    }
    
    m_json[name] = pointerBehavioursStrings;
}

template<>
void settings::Preferences::Get<settings::PointerBehaviourSetting>(settings::PointerBehaviourSetting &setting)
{
    using namespace sections;

    auto name = m_stringLookup.Get(settings::Key::PointerBehaviourSetting);

    if(m_json.contains(name))
    {
        auto behaviourStrings = m_json.at(name).get<std::unordered_map<std::string, std::string>>();
        PointerBehaviourMap pointerMap{};

        for(auto &[typeString, behaviourString] : behaviourStrings)
        {
            try 
            {
                auto type = m_stringLookup.Resolve<PointerType>(typeString);
                auto behaviour = m_stringLookup.Resolve<PointerBehaviour>(behaviourString);
                pointerMap[type] = behaviour;
            }
            catch(const program::JsonStringMissing&)
            {
                continue;
            }
        }
        
        setting.SetBehaviours(pointerMap);
        setting.Commit();
    }
}

template<>
void settings::Preferences::Set<settings::ShortcutsSetting>([[maybe_unused]] settings::ShortcutsSetting const &setting)
{
    // do nothing
}

template<>
void settings::Preferences::Get<settings::ShortcutsSetting>([[maybe_unused]] settings::ShortcutsSetting &setting)
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
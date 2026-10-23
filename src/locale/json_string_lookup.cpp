#include "locale/json_string_lookup.hpp"
#include "program/except.hpp"

locale::JsonStringLookup::JsonStringLookup()
{
    using namespace sections;

    m_pointerTypeStrings = {
        {{PointerType::Mouse, 0}, "left-mouse-button"},
        {{PointerType::Mouse, 1}, "right-mouse-button"},
        {{PointerType::Mouse, 2}, "middle-mouse-button"},
        {{PointerType::Mouse, 3}, "mouse-button-1"},
        {{PointerType::Mouse, 4}, "mouse-button-2"},
        {{PointerType::Pen, 0}, "pen-input"},
        {{PointerType::Pen, 1}, "pen-button"},
        {{PointerType::Touch, 0}, "touch-input"},
    };

    m_behaviourTypeStrings = {
        {PointerBehaviour::None, "none"},
        {PointerBehaviour::Painting, "painting"},
        {PointerBehaviour::Panning, "panning"},
    };

    m_settingsStrings = std::unordered_map<settings::Key, std::string> {
        {settings::Key::Default, "default"},
        {settings::Key::AutoRestoreFilesSetting, "auto-restore-files"},
        {settings::Key::DefaultOpenFiletypeSetting, "default-open-filetype"},
        {settings::Key::PointerBehaviourSetting, "pointer-behaviour"}
    };
}

template<>
std::string locale::JsonStringLookup::Get<sections::PointerMapKey>(sections::PointerMapKey value)
{
    return m_pointerTypeStrings[value];
}

template<>
sections::PointerMapKey locale::JsonStringLookup::Resolve<sections::PointerMapKey>(std::string value)
{
    for(auto &[key, string] : m_pointerTypeStrings)
    {
        if(string == value)
            return key;
    }
    throw program::JsonStringMissing("No string for PointerMapKey");
}

template<>
std::string locale::JsonStringLookup::Get<sections::PointerBehaviour>(sections::PointerBehaviour value)
{
    return m_behaviourTypeStrings[value];
}

template<>
sections::PointerBehaviour locale::JsonStringLookup::Resolve<sections::PointerBehaviour>(std::string value)
{
    for(auto &[type, string] : m_behaviourTypeStrings)
    {
        if(string == value)
            return type;
    }
    throw program::JsonStringMissing("No string for PointerBehaviour");
}

template<>
std::string locale::JsonStringLookup::Get<settings::Key>(settings::Key value)
{
    return m_settingsStrings[value];
}

template<>
settings::Key locale::JsonStringLookup::Resolve<settings::Key>(std::string value)
{
    for(auto &[type, string] : m_settingsStrings)
    {
        if(string == value)
            return type;
    }
    throw program::JsonStringMissing("No string for settings::Key");
}
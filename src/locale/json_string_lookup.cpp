#include "locale/json_string_lookup.hpp"
#include "program/except.hpp"

locale::JsonStringLookup::JsonStringLookup()
{
    using namespace sections;

    m_pointerTypeStrings = {
        {PointerType::LeftMouse, "left-mouse-button"},
        {PointerType::RightMouse, "right-mouse-button"},
        {PointerType::MiddleMouse, "middle-mouse-button"},
        {PointerType::Pen, "pen-input"},
        {PointerType::Touch, "touch-input"},
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
std::string locale::JsonStringLookup::Get<sections::PointerType>(sections::PointerType value)
{
    return m_pointerTypeStrings[value];
}

template<>
sections::PointerType locale::JsonStringLookup::Resolve<sections::PointerType>(std::string value)
{
    for(auto &[type, string] : m_pointerTypeStrings)
    {
        if(string == value)
            return type;
    }
    throw program::JsonStringMissing("No string for PointerType");
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
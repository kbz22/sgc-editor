#include "locale/shortcut_to_string.hpp"
#include <stdexcept>
#include <windows.h>

std::wstring locale::KeyToString(uint32_t key)
{
    LONG lParam = static_cast<LONG>(
        MapVirtualKeyW(key, MAPVK_VK_TO_VSC) << 16
    );

    wchar_t buffer[64]{};

    if (GetKeyNameTextW(lParam, buffer, ARRAYSIZE(buffer)) > 0)
        return buffer;

    return L"";
}

std::wstring locale::ShortcutToString(const win32_program::Shortcut& shortcut, program::ProgramContext &context)
{
    using namespace win32_program;

    if(shortcut.key == ShortcutKey::None && shortcut.modifier == ShortcutModifier::None)
        return L"";

    std::wstring result;
    auto &stringLookup = context.GetStringLookup();

    auto modifierStringIds = ShortcutManager::GetStringIdsForShortcutModifier(shortcut.modifier);
    for (const auto& stringId : modifierStringIds)
    {
        auto modifierString = stringLookup.Get(stringId);

        if(!modifierString.has_value())
            throw std::runtime_error("Failed to get modifier string for shortcut.");

        result += modifierString.value() + L"+";
    }

    auto keyStringId = ShortcutManager::GetStringIdForShortcutKey(shortcut.key);

    if(keyStringId.has_value())
    {
        auto keyString = stringLookup.Get(keyStringId.value());

        if(!keyString.has_value())
            throw std::runtime_error("Failed to get key string for shortcut.");

        result += keyString.value();
    }
    else
    {
        auto keyString = KeyToString(shortcut.key);
        result += keyString;
    }

    return result;
}

std::string locale::KeyToJsonString(uint32_t key)
{
    switch (key)
    {
        case VK_BACK:   return "Backspace";
        case VK_TAB:    return "Tab";
        case VK_RETURN: return "Enter";
        case VK_ESCAPE: return "Escape";
        case VK_SPACE:  return "Space";
        case VK_DELETE: return "Delete";
        case VK_INSERT: return "Insert";
        case VK_HOME:   return "Home";
        case VK_END:    return "End";
        case VK_PRIOR:  return "PageUp";
        case VK_NEXT:   return "PageDown";

        default:
            if (key >= 'A' && key <= 'Z')
                return std::string(1, static_cast<char>(key));

            if (key >= '0' && key <= '9')
                return std::string(1, static_cast<char>(key));

            return std::to_string(key);
    }
}

std::string locale::ShortcutToJsonString(const win32_program::Shortcut& shortcut)
{
    using namespace win32_program;
    
    std::string result;
    auto modifier = static_cast<uint32_t>(shortcut.modifier);

    if (modifier & static_cast<uint32_t>(ShortcutModifier::Ctrl))
        result += "Ctrl+";

    if (modifier & static_cast<uint32_t>(ShortcutModifier::Shift))
        result += "Shift+";

    if (modifier & static_cast<uint32_t>(ShortcutModifier::Alt))
        result += "Alt+";

    result += KeyToJsonString(shortcut.key);

    return result;
}

win32_program::Shortcut locale::JsonStringToShortcut(const std::string &string)
{
    using namespace win32_program;

    ShortcutModifier modifier = ShortcutModifier::None;
    size_t start = 0;

    while (true)
    {
        const size_t separator = string.find('+', start);

        // Last component is the key.
        if (separator == std::string_view::npos)
        {
            std::string keyString = string.substr(start);

            if (keyString.empty())
                throw std::runtime_error("Invalid shortcut: missing key");

            return Shortcut{
                .modifier = modifier,
                .key = JsonStringToKey(keyString)
            };
        }

        const std::string_view modifierString =
            string.substr(start, separator - start);

        if (modifierString == "Ctrl")
        {
            modifier = modifier | ShortcutModifier::Ctrl;
        }
        else if (modifierString == "Alt")
        {
            modifier = modifier | ShortcutModifier::Alt;
        }
        else if (modifierString == "Shift")
        {
            modifier = modifier | ShortcutModifier::Shift;
        }
        else
        {
            throw std::runtime_error("Invalid shortcut modifier");
        }

        start = separator + 1;
    }
}

uint32_t locale::JsonStringToKey(const std::string &string)
{
    if (string == "Backspace")
        return VK_BACK;

    if (string == "Tab")
        return VK_TAB;

    if (string == "Enter")
        return VK_RETURN;

    if (string == "Escape")
        return VK_ESCAPE;

    if (string == "Space")
        return VK_SPACE;

    if (string == "Delete")
        return VK_DELETE;

    if (string == "Insert")
        return VK_INSERT;

    if (string == "Home")
        return VK_HOME;

    if (string == "End")
        return VK_END;

    if (string == "PageUp")
        return VK_PRIOR;

    if (string == "PageDown")
        return VK_NEXT;

    // A-Z
    if (string.size() == 1 &&
        string[0] >= 'A' &&
        string[0] <= 'Z')
    {
        return static_cast<uint32_t>(string[0]);
    }

    // 0-9
    if (string.size() == 1 &&
        string[0] >= '0' &&
        string[0] <= '9')
    {
        return static_cast<uint32_t>(string[0]);
    }

    throw std::runtime_error("Invalid shortcut key");
}
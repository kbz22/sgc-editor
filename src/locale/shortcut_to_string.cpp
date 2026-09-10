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
    std::wstring result;
    auto &stringLookup = context.GetStringLookup();

    auto modifierStringIds = win32_program::ShortcutManager::GetStringIdsForShortcutModifier(shortcut.modifier);
    for (const auto& stringId : modifierStringIds)
    {
        auto modifierString = stringLookup.Get(stringId);

        if(!modifierString.has_value())
            throw std::runtime_error("Failed to get modifier string for shortcut.");

        result += modifierString.value() + L"+";
    }

    auto keyStringId = win32_program::ShortcutManager::GetStringIdForShortcutKey(shortcut.key);

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
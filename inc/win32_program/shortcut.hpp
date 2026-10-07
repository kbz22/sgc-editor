#pragma once

#include <cstdint>
#include <unordered_map>
#include <windows.h>

namespace win32_program {

    enum class ShortcutModifier : uint32_t
    {
        None = 0x00,
        Ctrl = 0x01,
        Alt = 0x02,
        Shift = 0x04
    };

    enum ShortcutKey : uint32_t
    {
        None = 0,
        Ctrl = VK_CONTROL,
        Alt = VK_MENU,
        Shift = VK_SHIFT,
        Delete = VK_DELETE,
        Insert = VK_INSERT,
        Home = VK_HOME,
        End = VK_END,
        PageUp = VK_PRIOR,
        PageDown = VK_NEXT,
        ArrowUp = VK_UP,
        ArrowDown = VK_DOWN,
        ArrowLeft = VK_LEFT,
        ArrowRight = VK_RIGHT,
    };

    enum class ShortcutContext : uint32_t
    {
        Global,
        MapEditor,
        MapEditorSelection,
    };

    struct Shortcut
    {        
        ShortcutModifier modifier = ShortcutModifier::None;
        uint32_t key = ShortcutKey::None;

        bool operator==(const Shortcut&) const = default;
    };

    constexpr ShortcutModifier operator|(ShortcutModifier lhs, ShortcutModifier rhs)
    {
        return static_cast<ShortcutModifier>(
            static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs)
        );
    }

    constexpr ShortcutModifier operator&(ShortcutModifier lhs, ShortcutModifier rhs)
    {
        return static_cast<ShortcutModifier>(
            static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs)
        );
    }

    constexpr bool HasFlag(ShortcutModifier value, ShortcutModifier flag)
    {
        return (value & flag) == flag;
    }

}

namespace std
{
    template<>
    struct hash<win32_program::Shortcut>
    {
        std::size_t operator()(const win32_program::Shortcut& shortcut) const
        {
            return std::hash<uint32_t>()(static_cast<uint32_t>(shortcut.modifier)) ^ std::hash<uint32_t>()(shortcut.key);
        }
    };
}
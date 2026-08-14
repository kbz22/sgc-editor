#pragma once

#include <cstdint>
#include <unordered_map>

namespace program {

    enum class ShortcutModifier : uint32_t
    {
        None = 0x00,
        Ctrl = 0x01,
        Alt = 0x02,
        Shift = 0x04,
        Delete = 0x08,
        Insert = 0x10
    };

    enum class ShortcutContext : uint32_t
    {
        Global,
        MapEditor,
        MapEditorSelection,
    };

    struct Shortcut
    {        
        ShortcutModifier modifier;
        uint32_t key;

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
    struct hash<program::Shortcut>
    {
        std::size_t operator()(const program::Shortcut& shortcut) const
        {
            return std::hash<uint32_t>()(static_cast<uint32_t>(shortcut.modifier)) ^ std::hash<uint32_t>()(shortcut.key);
        }
    };
}
#pragma once

#include <cstdint>
#include <unordered_map>

namespace program {

    enum class ShortcutModifier : uint32_t
    {
        None = 0,
        Ctrl = 1,
        Alt = 2,
        Shift = 4
    };

    enum class ShortcutContext : uint32_t
    {
        Global,
        MapEditor
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
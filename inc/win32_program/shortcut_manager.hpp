#pragma once

#include <windows.h>

#include <unordered_map>
#include <vector>
#include <cstdint>

#include "action/action_types.hpp"

namespace win32_program
{
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

    class ShortcutManager    
    {
        private:
            HINSTANCE hInstance;
            HWND hwnd;
            std::unordered_map<
                ShortcutContext, 
                std::unordered_map<Shortcut, action::ActionType, 
                    std::hash<Shortcut>
                >
            > m_shortcuts;

        public:
            ShortcutManager(
                HINSTANCE hInstance,
                HWND hwnd
            );

            void RegisterShortcut(
                const Shortcut& shortcut,
                ShortcutContext context,
                action::ActionType actionId
            );

            action::ActionType GetActionForShortcut(
                const Shortcut& shortcut,
                ShortcutContext context
            ) const;

            static ShortcutModifier GetShortcutModifierFromKeyState();
    };
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
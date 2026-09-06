#pragma once

#include <windows.h>

#include <unordered_map>
#include <vector>
#include <cstdint>

#include "action/action_types.hpp"
#include "win32_program/shortcut.hpp"

namespace win32_program
{
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
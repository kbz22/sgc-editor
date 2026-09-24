#pragma once

#include <windows.h>

#include <unordered_map>
#include <vector>
#include <cstdint>
#include <optional>

#include "action/action_types.hpp"
#include "win32_program/shortcut.hpp"
#include "locale/stringid.hpp"

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

            void ReplaceShortcut(
                action::ActionType actionId,
                ShortcutContext context,
                const Shortcut& oldShortcut,
                const Shortcut& newShortcut
            );

            action::ActionType GetActionForShortcut(
                const Shortcut& shortcut,
                ShortcutContext context
            ) const;

            std::vector<Shortcut> GetShortcutsForAction(action::ActionType actionId) const;

            static ShortcutModifier GetShortcutModifierFromKeyState();
            static std::optional<locale::StringId> GetStringIdForShortcutKey(uint32_t key);
            static std::vector<locale::StringId> GetStringIdsForShortcutModifier(ShortcutModifier modifier);
    };
}

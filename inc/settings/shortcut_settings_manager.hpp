#pragma once

#include "win32_program/shortcut.hpp"
#include "win32_program/shortcut_manager.hpp"
#include "action/action.hpp"
#include "locale/stringid.hpp"

#include <unordered_map>

namespace settings
{
    class ProgramContext;

    struct ShortcutEntry
    {
        int index = 0;        
        win32_program::Shortcut shortcut = win32_program::Shortcut{};
        action::ActionType actionType = action::ActionType::Default;
        win32_program::ShortcutContext context = win32_program::ShortcutContext::Global;
        std::wstring actionNameString = L"";
        std::wstring shortcutString = L"";
    };

    class ShortcutSettingsManager
    {
        private:
            std::unordered_map<int, ShortcutEntry> m_shortcutEntries;
            ShortcutEntry m_editedShortcut;

        public:        
            void AddShortcutEntry(const ShortcutEntry& entry);
            void RemoveShortcutEntry(int index);            
            ShortcutEntry* GetShortcutEntry(int index);

            void SetEditedShortcutEntry(const ShortcutEntry& entry);
            void SetEditedShortcut(const win32_program::Shortcut& shortcut, program::ProgramContext& programContext);
            ShortcutEntry* GetEditedShortcut();

            void CommitEdit(win32_program::ShortcutManager& shortcutManager);
            void CancelEdit();
            bool IsEditing();
    };
}
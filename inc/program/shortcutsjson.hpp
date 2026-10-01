#pragma once

#include <nlohmann/json.hpp>
#include <vector>
#include <filesystem>

#include "win32_program/shortcut.hpp"
#include "action/action_types.hpp"

namespace win32_program
{
    class ShortcutManager;
}

namespace program 
{
    struct ShortcutJsonData
    {
        action::ActionType action;
        std::vector<win32_program::Shortcut> shortcuts;
    };

    class ShortcutsJson
    {
        private:
            nlohmann::json m_json{};            
            std::wstring m_shortcutsFilename = L"shortcuts.json";
            const std::string m_shortcutsString = "shortcuts";

        public:
            void Load();
            void Save();

            void RebuildShortcutList(std::vector<ShortcutJsonData> const &shortcutsData);            
            void ReplaceShortcuts(win32_program::ShortcutManager &shortcutManager);

    };
}
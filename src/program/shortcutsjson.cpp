#include "program/shortcutsjson.hpp"
#include "program/program.hpp"
#include "win32_program/shortcut_manager.hpp"
#include "win32_helpers/file_helpers.hpp"
#include "locale/shortcut_to_string.hpp"
#include "locale/action_json_string_lookup.hpp"
#include <fstream>

void program::ShortcutsJson::Load()
{
    std::filesystem::path path = win32_helpers::GetPreferencesDirectory();
    path /= m_shortcutsFilename;
    std::ifstream file(path);

    if (!file)
    {
        throw std::runtime_error("Failed to open preferences file for reading");
    }
    
    file >> m_json;
}

void program::ShortcutsJson::Save()
{
    std::filesystem::path path = win32_helpers::GetPreferencesDirectory();
    path /= m_shortcutsFilename;
    std::ofstream file(path);

    if (!file)
    {
        throw std::runtime_error("Failed to open preferences file for writing");
    }

    file << m_json.dump(4);
}

void program::ShortcutsJson::RebuildShortcutList(std::vector<ShortcutJsonData> const &shortcutsData)
{
    for(auto &data : shortcutsData)
    {
        std::vector<std::string> shortcutStrings{};
        for(auto &shortcut : data.shortcuts)
            shortcutStrings.push_back(locale::ShortcutToJsonString(shortcut));

        m_json[m_shortcutsString][locale::ActionJsonStringLookup::Get(data.action)] = shortcutStrings;
    }
}

void program::ShortcutsJson::ReplaceShortcuts(win32_program::ShortcutManager &shortcutManager, action::ActionManager &actionManager)
{
    // auto allShortcuts = m_json[m_shortcutsString].get<std::unordered_map<std::string, std::vector<std::string>>>();

    for(auto &[actionName, shortcutStrings] : m_json[m_shortcutsString].items())
    {
        auto actionId = locale::ActionJsonStringLookup::Get(actionName);
        auto context = actionManager.Find(actionId)->GetShortcutContext();

        for(auto &shortcutString : shortcutStrings)
        {
            auto shortcut = locale::JsonStringToShortcut(shortcutString);
            shortcutManager.RegisterShortcut(
                shortcut,
                context,
                actionId
            );
        }        
    }
    
}
#include "program/shortcutsjson.hpp"
#include "program/program.hpp"
#include "win32_program/shortcut_manager.hpp"
#include "win32_helpers/file_helpers.hpp"
#include "locale/shortcut_to_string.hpp"
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
        for(auto &shortcut : data.shortcuts)
            m_json[m_shortcutsString][static_cast<int>(data.action)] = locale::ShortcutToString(shortcut, program::GetProgramContext());
}

void program::ShortcutsJson::ReplaceShortcuts(win32_program::ShortcutManager &shortcutManager)
{
    // I need to think about this
}
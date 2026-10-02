#pragma once

#include "win32_program/shortcut.hpp"
#include "win32_program/shortcut_manager.hpp"
#include "program/program.hpp"
#include <string>

namespace locale
{
    std::wstring KeyToString(uint32_t key);
    std::wstring ShortcutToString(const win32_program::Shortcut& shortcut, program::ProgramContext &context);
    
    std::string KeyToJsonString(uint32_t key);
    std::string ShortcutToJsonString(const win32_program::Shortcut& shortcut);
}
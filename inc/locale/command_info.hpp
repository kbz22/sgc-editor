#pragma once

#include "win32_program/windows_controls.hpp"
#include <string>

namespace locale
{
    struct CommandInfo
    {
        win32_program::CommandId id;
        std::wstring tooltip;
    };

}
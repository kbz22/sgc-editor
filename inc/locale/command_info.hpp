#pragma once

#include "win32_program/windows_controls.hpp"
#include "locale/stringid.hpp"

namespace locale
{
    struct CommandInfo
    {
        win32_program::CommandId id;
        locale::StringId tooltip;
    };

}
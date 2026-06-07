#pragma once

#include "win32_program/windows_init.hpp"
#include "types.hpp"

namespace win32_program
{
    enum class ControlId : types::ctrid_t
    {
        Menu = 1001,
        Toolbar = 1002
    };

    enum class CommandId : types::cmdid_t
    {
        MenuFile = 2001,
        MenuEdit = 2002,

        FileNew = 3001,
        FileOpen = 3002,
        FileSave = 3003
    };

    void SetupMenuBar(Win32Context& context);

    void SetupToolbar(Win32Context& context);

}
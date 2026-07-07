#pragma once

/* #include "win32_program/windows_init.hpp" */
#include "types.hpp"

namespace win32_program
{
    enum class ControlId : types::ctrid_t
    {
        Menu = 1001,
        Toolbar = 1002,

        LayerList = 1003,
        PackageView = 1004,
        MapView = 1005,
        TilesetView = 1006,

        SplitLeft = 2001,
        SplitRight = 2002,
        SplitBottom = 2003
    };

    enum class CommandId : types::cmdid_t
    {
        MenuFile = 1001,
        MenuEdit = 1002,

        FileNew = 2001,
        FileOpen = 2002,
        FileSave = 2003
    };
}
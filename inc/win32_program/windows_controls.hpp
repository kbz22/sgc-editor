#pragma once

/* #include "win32_program/windows_init.hpp" */
#include "types.hpp"

namespace win32_program
{
    enum class ControlId : types::ctrid_t
    {
        MenuRebar = 1001,        
        MenuToolbar = 1002,
        ToolbarRebar = 1003,
        ToolbarToolbar = 1004,

        LayerList = 1005,
        PackageView = 1006,
        MapView = 1007,
        TilesetView = 1008,

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
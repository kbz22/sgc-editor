#pragma once

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
        StatusView = 1009,

        SplitTilesetMap = 2001,
        SplitLayerPackage = 2002,
        SplitLayerMap = 2003
    };
    
}
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

        SplitTilesetMap = 2001,
        SplitLayerPackage = 2002,
        SplitLayerMap = 2003
    };

    enum class CommandId : types::cmdid_t
    {
        NotApplicable,

        MenuFile = 1001,
        MenuEdit = 1002,

        FileNew = 2001,
        FileOpen = 2002,
        FileSave = 2003,

        EditUndo = 3001,
        EditRedo = 3002,

        LayerAdd = 4001,
        LayerRemove = 4002,
        LayerMoveUp = 4003,
        LayerMoveDown = 4004,

        EditorLayerModeNonActiveTransparent = 5001,
        EditorLayerModeSingleLayer = 5002,
        EditorLayerModeSingleImage = 5003,

        EditorChunkModeFixedSize = 6001,
        EditorChunkModeFree = 6002,

        EditorPaintModeSingle = 7001,
        EditorPaintModeRectangle = 7002,
        EditorPaintModeLine = 7003,
        EditorPaintModeFill = 7004
    };
    
}
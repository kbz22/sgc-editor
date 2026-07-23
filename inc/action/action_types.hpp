#pragma once

namespace action {

    enum class ActionType
    {
        Default = 0,

        MenuFile = 1001,
        NewFile = 1002,
        OpenFile,
        SaveFile,
        CloseFile,

        MenuEdit = 2001,
        Undo,
        Redo,

        MenuMap = 3001,
        AddLayer,
        RemoveLayer,
        MoveLayer,

        ChunkModeFixedSize,
        ChunkModeFree,

        MenuView = 4001,
        LayerModeMultilayer,
        LayerModeSingleLayer,
        LayerModeSingleImage        
    };
}
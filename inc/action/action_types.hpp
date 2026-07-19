#pragma once

namespace action {

    enum class ActionType
    {
        Default = 0,

        NewFile = 1001,
        OpenFile,
        SaveFile,
        CloseFile,

        Undo,
        Redo,

        AddLayer,
        RemoveLayer,
        MoveLayerUp,
        MoveLayerDown,

        LayerModeMultilayer,
        LayerModeSingleLayer,
        LayerModeSingleImage,

        ChunkModeFixedSize,
        ChunkModeFree,
    };
}
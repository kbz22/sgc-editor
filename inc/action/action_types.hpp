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
        
        PaintModeBrush,
        PaintModeRectangle,
        PaintModeFill,
        PaintModeSelect,
        PaintModeEraser,

        MenuMap = 3001,
        AddLayer,
        RemoveLayer,
        MoveLayerUp,
        MoveLayerDown,

        ChunkModeFixedSize,
        ChunkModeFree,

        MenuView = 4001,
        LayerModeMultilayer,
        LayerModeSingleLayer,
        LayerModeSingleImage,

        MenuHelp = 5001,
        HelpAbout,
        HelpGithubHyperlink
    };
}
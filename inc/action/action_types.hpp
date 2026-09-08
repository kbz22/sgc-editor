#pragma once

namespace action {

    enum class ActionType
    {
        Default = 0,

        MenuFile = 1001,        
        NewFile = 1002,
        NewMapDocument,
        NewTilesetDocument,
        NewPackage,
        OpenFile,
        SaveFile,
        SaveAs,
        ExportFile,
        ExportAsImage,
        ExportAsPackage,
        CloseFile,
        Settings,

        MenuEdit = 2001,
        Undo,
        Redo,
        SelectionCut,
        SelectionCopy,
        SelectionPaste,
        SelectionClear,
        SelectionMove,
        
        PaintModeBrush,
        PaintModeRectangle,
        PaintModeFill,
        PaintModeSelect,
        
        EraseModeClearTile,
        EraseModeDeleteChunk,

        SelectSingleLayerMode,
        SelectAllLayersMode,
        SelectVisibleLayersMode,

        MenuMap = 3001,
        AddLayer,
        RemoveLayer,
        MoveLayerUp,
        MoveLayerDown,

        ChunkModeFixedSize,
        ChunkModeFree,

        MenuView = 4001,
        SelectZoom,
        ResetZoom,
        ZoomIn,
        ZoomOut,
        LayerModeMultilayer,
        LayerModeSingleLayer,
        LayerModeSingleImage,        
        GridModeTile,
        GridModeChunk,

        MenuHelp = 5001,
        HelpAbout,
        HelpGithubHyperlink,

        ChangeActiveLayerUp = 6001,
        ChangeActiveLayerDown,
        ChangeActiveLayerTop,
        ChangeActiveLayerBottom
    };
}
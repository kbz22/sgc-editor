#pragma once

namespace locale
{
    enum class StringId
    {        
        TextMissing,        

        TooltipFileNew,
        TooltipFileOpen,
        TooltipFileSave,
        TooltipEditUndo,
        TooltipEditRedo,
        TooltipLayerAdd,
        TooltipLayerRemove,
        TooltipLayerMoveUp,
        TooltipLayerMoveDown,
        TooltipEditorLayerModeMultilayer,
        TooltipEditorLayerModeSingleLayer,
        TooltipEditorLayerModeSingleImage,
        TooltipEditorChunkModeFixedSize,
        TooltipEditorChunkModeFree,

        NameFile,
        NameEdit,
        NameMap,
        NameView,
        NameHelp,

        NameNewFile,
        NameOpenFile,
        NameSaveFile,
        NameUndo,
        NameRedo,
        NameAddLayer,
        NameRemoveLayer,
        NameMoveLayerUp,
        NameMoveLayerDown,
        NameSelectEditorLayerMode,
        NameEditorLayerModeMultilayer,
        NameEditorLayerModeSingleLayer,
        NameEditorLayerModeSingleImage,
        NameSelectEditorChunkMode,
        NameEditorChunkModeFixedSize,
        NameEditorChunkModeFree
    };
}
#include "locale/string_lookup.hpp"
#include <stdexcept>

locale::StringLookup::StringLookup()
{
    m_strings =
    {
        {StringId::WindowTitle, L"SGC Map Editor"},

        {StringId::TooltipNewMapDocument,  L"New map"},
        {StringId::TooltipFileOpen, L"Open file"},
        {StringId::TooltipFileSave, L"Save file"},
        {StringId::TooltipEditUndo, L"Undo"},
        {StringId::TooltipEditRedo, L"Redo"},
        {StringId::TooltipLayerAdd, L"Add layer"},
        {StringId::TooltipLayerRemove, L"Remove layer"},
        {StringId::TooltipLayerMoveUp, L"Move active layer up"},
        {StringId::TooltipLayerMoveDown, L"Move active layer down"},
        {StringId::TooltipEditorLayerModeMultilayer, L"Transparent mode"},
        {StringId::TooltipEditorLayerModeSingleLayer, L"Single layer mode"},
        {StringId::TooltipEditorLayerModeSingleImage, L"Single image mode"},
        {StringId::TooltipEditorChunkModeFixedSize, L"Fixed chunks mode"},
        {StringId::TooltipEditorChunkModeFree, L"Free chunks mode"},
        {StringId::TooltipEditorPaintModeBrush, L"Brush mode"},
        {StringId::TooltipEditorPaintModeRectangle, L"Rectangle draw mode"},
        {StringId::TooltipEditorPaintModeFill, L"Fill mode"},
        {StringId::TooltipEditorPaintModeSelect, L"Selection mode"},
        {StringId::TooltipEditorPaintModeEraser, L"Eraser"},
        {StringId::TooltipEditorResetZoom, L"Set zoom to 100%"},
        {StringId::TooltipEditorEraseModeClearTile, L"Set a tile to empty"},
        {StringId::TooltipEditorEraseModeDeleteChunk, L"Delete a chunk"},
        {StringId::TooltipEditorChunkLineGrid, L"Show chunk grid"},
        {StringId::TooltipEditorTileLineGrid, L"Show tile grid"},
        {StringId::TooltipEditorSelectModeSingleLayer, L"Active layer selection"},
        {StringId::TooltipEditorSelectModeAllLayers, L"All layers selection"},
        {StringId::TooltipEditorSelectModeVisibleLayers, L"Visible layers selection"},
        {StringId::TooltipEditorSelectionCut, L"Cut selection"},
        {StringId::TooltipEditorSelectionCopy, L"Copy selection"},
        {StringId::TooltipEditorSelectionPaste, L"Paste selection"},
        {StringId::TooltipEditorSelectionClear, L"Clear selection"},
        {StringId::TooltipEditorSelectionMove, L"Move selection"},

        {StringId::NameFile, L"File"},
        {StringId::NameEdit, L"Edit"},
        {StringId::NameMap, L"Map"},
        {StringId::NameView, L"View"},
        {StringId::NameHelp, L"Help"},

        {StringId::NameNewFile, L"New"},
        {StringId::NameNewMapDocument, L"Map"},
        {StringId::NameNewTilesetDocument, L"Tileset"},
        {StringId::NameOpenFile, L"Open"},
        {StringId::NameSaveFile, L"Save"},
        {StringId::NameCloseFile, L"Close"},
        {StringId::NameUndo, L"Undo"},
        {StringId::NameRedo, L"Redo"},
        {StringId::NameAddLayer, L"Add layer"},
        {StringId::NameRemoveLayer, L"Remove layer"},
        {StringId::NameMoveLayerUp, L"Move layer up"},
        {StringId::NameMoveLayerDown, L"Move layer down"},
        {StringId::NameSelectEditorLayerMode, L"Layer mode"},
        {StringId::NameEditorLayerModeMultilayer, L"Transparent"},
        {StringId::NameEditorLayerModeSingleLayer, L"Single layer"},
        {StringId::NameEditorLayerModeSingleImage, L"Single image"},
        {StringId::NameSelectEditorChunkMode, L"Chunk mode"},
        {StringId::NameEditorChunkModeFixedSize, L"Fixed size chunk"},
        {StringId::NameEditorChunkModeFree, L"Free chunk"},
        {StringId::NamePaintModeBrush, L"Brush"},
        {StringId::NamePaintModeRectangle, L"Rectangle"},
        {StringId::NamePaintModeFill, L"Fill"},
        {StringId::NamePaintModeSelect, L"Select"},
        {StringId::NamePaintModeEraser, L"Eraser"},        
        {StringId::NameZoomIn, L"Zoom in"},
        {StringId::NameZoomOut, L"Zoom out"},
        {StringId::NameEraserModeClearTile, L"Clear tile"},
        {StringId::NameEraserModeDeleteChunk, L"Delete chunk"},
        {StringId::NameEditorChunkLineGrid, L"Chunk grid"},
        {StringId::NameEditorTileLineGrid, L"Tile grid"},
        {StringId::NameEditorSelectionModeSingleLayer, L"Single layer selection"},
        {StringId::NameEditorSelectionModeAllLayers, L"All layers selection"},
        {StringId::NameEditorSelectionModeVisibleLayers, L"Visible layers selection"},
        {StringId::NameEditorSelectionCut, L"Cut"},
        {StringId::NameEditorSelectionCopy, L"Copy"},
        {StringId::NameEditorSelectionPaste, L"Paste"},
        {StringId::NameEditorSelectionClear, L"Clear"},
        {StringId::NameEditorSelectionMove, L"Move"},

        {StringId::NameTilesetFile, L"Tileset file"},
        {StringId::NameMapFile, L"Map file"},
        {StringId::NameImageFile, L"Image file"},
        {StringId::NameAllFiles, L"All files"},
        {StringId::NamePackageFile, L"Package file"},

        {StringId::TextMissing, L"Text missing"}
    };
}

const std::optional<std::wstring> locale::StringLookup::Get(StringId id) const
{
    auto it = m_strings.find(id);

    std::optional<std::wstring> result = std::nullopt;

    if (it != m_strings.end())
    {
        result = std::optional<std::wstring>{it->second};
    }

    return result;
}
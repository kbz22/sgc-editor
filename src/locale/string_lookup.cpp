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
        {StringId::TooltipEditorChunkModeFixedSize, L"Fixed size chunk mode"},
        {StringId::TooltipEditorChunkModeFree, L"Free chunk mode"},
        {StringId::TooltipEditorPaintModeBrush, L"Brush"},
        {StringId::TooltipEditorPaintModeRectangle, L"Rectangle"},
        {StringId::TooltipEditorPaintModeFill, L"Fill"},
        {StringId::TooltipEditorPaintModeSelect, L"Select"},
        {StringId::TooltipEditorPaintModeEraser, L"Eraser"},

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
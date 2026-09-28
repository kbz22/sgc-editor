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
        {StringId::TooltipSettings, L"Settings"},        

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
        {StringId::NameEditorLayerModeMultilayer, L"Transparent mode"},
        {StringId::NameEditorLayerModeSingleLayer, L"Single layer mode"},
        {StringId::NameEditorLayerModeSingleImage, L"Single image mode"},
        {StringId::NameSelectEditorChunkMode, L"Chunk mode"},
        {StringId::NameEditorChunkModeFixedSize, L"Fixed chunks mode"},
        {StringId::NameEditorChunkModeFree, L"Free chunks mode"},
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
        {StringId::NameSaveAs, L"Save As"},
        {StringId::NameExportFile, L"Export"},
        {StringId::NameExportAsImage, L"Image"},
        {StringId::NameExportAsPackage, L"Package"},
        {StringId::NameExportAsMapFile, L"Map file"},
        {StringId::NameNewPackage, L"Package"},
        {StringId::NameSettings, L"Settings"},
        {StringId::NameHelpAbout, L"About"},
        {StringId::NameHelpGithub, L"Github repository"},
        {StringId::NameChangeLayerDown, L"Layer below"},
        {StringId::NameChangeLayerUp, L"Layer above"},
        {StringId::NameChangeLayerTop, L"Top layer"},
        {StringId::NameChangeLayerBottom, L"Bottom layer"},

        {StringId::NameTilesetFile, L"Tileset file"},
        {StringId::NameMapFile, L"Map file"},
        {StringId::NameImageFile, L"Image file"},
        {StringId::NameAllFiles, L"All files"},
        {StringId::NamePackageFile, L"Package file"},

        {StringId::SettingsNameGeneral, L"General"},
        {StringId::SettingsNameShortcuts, L"Shortcuts"},

        {StringId::ShortcutNameCtrl, L"Ctrl"},
        {StringId::ShortcutNameAlt, L"Alt"},
        {StringId::ShortcutNameShift, L"Shift"},
        {StringId::ShortcutNameInsert, L"Insert"},
        {StringId::ShortcutNameDelete, L"Delete"},
        {StringId::ShortcutNamePageUp, L"Page Up"},
        {StringId::ShortcutNamePageDown, L"Page Down"},
        {StringId::ShortcutNameHome, L"Home"},
        {StringId::ShortcutNameEnd, L"End"},
        {StringId::ShortcutNameArrowUp, L"Arrow Up"},
        {StringId::ShortcutNameArrowDown, L"Arrow Down"},
        {StringId::ShortcutNameArrowLeft, L"Arrow Left"},
        {StringId::ShortcutNameArrowRight, L"Arrow Right"},

        {StringId::ShortcutNameNewMap, L"New map"},
        {StringId::ShortcutNameNewTileset, L"New tileset"},
        {StringId::ShortcutNameNewPackage, L"New package"},
        {StringId::ShortcutNameExportAsImage, L"Export as image"},

        {StringId::SettingsGeneralStartupName, L"Startup"},
        {StringId::SettingsGeneralAutoRestoreOpenFiles, L"Automatically restore last session"},
        {StringId::SettingsGeneralFileName, L"File"},
        {StringId::SettingsGeneralDefaultOpenFileType, LR"(Default "Open File" type: )"},
        {StringId::SettingsShortcutSetNew, L"Press and hold the new shortcut"},
        {StringId::SettingsShortcutFilter, L"Filter: "},
        {StringId::SettingsShortcutActionName, L"Action"},
        {StringId::SettingsShortcutShortcutName, L"Shortcut"},

        {StringId::DialogOk, L"OK"},
        {StringId::DialogCreate, L"Create"},
        {StringId::DialogCancel, L"Cancel"},
        {StringId::DialogApply, L"Apply"},

        {StringId::NewDialogNameName, L"Name:"},
        {StringId::NewDialogTilesetName, L"Tileset:"},
        {StringId::NewDialogNewName, L"New..."},
        {StringId::NewDialogPackageName, L"Package:"},

        {StringId::NewMapDialogIncludeInPackage, L"Include the new map in a package file"},

        {StringId::NewTilesetDialogImageName, L"Image:"},
        {StringId::NewTilesetDialogTileWidthName, L"Tile width:"},
        {StringId::NewTilesetDialogTileHeightName, L"Tile height:"},
        {StringId::NewTilesetDialogIncludeInPackage, L"Include the new tileset in a package file"},

        {StringId::ErrorName, L"Error"},
        {StringId::UserErrorNoMapName, L"Please provide a name for the map."},
        {StringId::UserErrorNoTileset, L"Please select a tileset."},
        {StringId::UserErrorNoPackage, L"Please select a package."},
        {StringId::UserErrorNoPackageName, L"Please provide a name for the package"},        
        {StringId::UserErrorNoTilesetName, L"Please provide a name for the tileset"},
        {StringId::UserErrorNoImage, L"Please select an image."},
        {StringId::UserErrorWrongTileSize, L"Tile width and height must be greater than zero."},

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

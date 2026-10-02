#include "locale/action_json_string_lookup.hpp"

std::unordered_map<action::ActionType, std::string> locale::ActionJsonStringLookup::m_strings{
    {action::ActionType::NewMapDocument, "new-map-document"},
    {action::ActionType::NewTilesetDocument, "new-tileset-document"},
    {action::ActionType::NewPackage, "new-package-file"},
    {action::ActionType::OpenFile, "open-file"},
    {action::ActionType::SaveFile, "save-file"},
    {action::ActionType::SaveAs, "save-file-as"},
    {action::ActionType::ExportAsImage, "export-as-image"},
    {action::ActionType::ExportAsPackage, "export-as-package"},
    {action::ActionType::CloseFile, "close-file"},
    {action::ActionType::Settings, "settings"},
    {action::ActionType::Undo, "undo"},
    {action::ActionType::Redo, "redo"},
    {action::ActionType::SelectionCut, "edit-cut"},
    {action::ActionType::SelectionCopy, "edit-copy"},
    {action::ActionType::SelectionClear, "clear-selection"},
    {action::ActionType::SelectionMove, "move-selection"},
    {action::ActionType::SelectionPaste, "edit-paste"},
    {action::ActionType::PaintModeBrush, "brush-paint-mode"},
    {action::ActionType::PaintModeFill, "fill-paint-mode"},
    {action::ActionType::PaintModeRectangle, "rectangle-paint-mode"},
    {action::ActionType::PaintModeSelect, "select-paint-mode"},
    {action::ActionType::EraseModeClearTile, "delete-tile-mode"},
    {action::ActionType::EraseModeDeleteChunk, "delete-chunk-mode"},
    {action::ActionType::SelectSingleLayerMode, "single-layer-selection-mode"},
    {action::ActionType::SelectVisibleLayersMode, "visible-layers-selection-mode"},
    {action::ActionType::SelectAllLayersMode, "all-layers-selection-mode"},
    {action::ActionType::AddLayer, "add-layer"},
    {action::ActionType::RemoveLayer, "remove-layer"},
    {action::ActionType::MoveLayerUp, "move-layer-up"},
    {action::ActionType::MoveLayerDown, "move-layer-down"},
    {action::ActionType::ChunkModeFixedSize, "fixed-chunks-mode"},
    {action::ActionType::ChunkModeFree, "dynamic-chunks-mode"},
    {action::ActionType::SelectZoom, "zoom-select"}, //! does this even work with shortcuts?
    {action::ActionType::ResetZoom, "zoom-reset"},
    {action::ActionType::ZoomIn, "zoom-in"},
    {action::ActionType::ZoomOut, "zoom-out"},
    {action::ActionType::LayerModeMultilayer, "layer-mode-transparent-layers"},
    {action::ActionType::LayerModeSingleImage, "layer-mode-single-image"},
    {action::ActionType::LayerModeSingleLayer, "layer-mode-single-layer"},
    {action::ActionType::GridModeTile, "tile-grid"},
    {action::ActionType::GridModeChunk, "chunk-grid"},
    {action::ActionType::ChangeActiveLayerUp, "active-layer-up"},
    {action::ActionType::ChangeActiveLayerDown, "active-layer-down"},
    {action::ActionType::ChangeActiveLayerTop, "active-layer-top"},
    {action::ActionType::ChangeActiveLayerBottom, "active-layer-bottom"},
};

std::string locale::ActionJsonStringLookup::Get(action::ActionType actionId)
{
    return m_strings[actionId];
}

action::ActionType locale::ActionJsonStringLookup::Get(std::string key)
{
    for(auto &[actionId, jsonKey] : m_strings)
    {
        if(jsonKey == key){
            return actionId;
        }
    }
    return action::ActionType::Default;
}
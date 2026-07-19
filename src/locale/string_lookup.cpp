#include "locale/string_lookup.hpp"
#include <stdexcept>

locale::StringLookup::StringLookup()
{
    m_strings =
    {
        {StringId::TooltipFileNew,  L"New file"},
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

        {StringId::NameFile, L"File"},
        {StringId::NameEdit, L"Edit"},
        {StringId::NameNewFile, L"New"},
        {StringId::NameOpenFile, L"Open"},
        {StringId::NameSaveFile, L"Save"},
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
        {StringId::NameEditorChunkModeFree, L"Free chunk"}
    };
}

const std::wstring& locale::StringLookup::Get(StringId id) const
{
    auto it = m_strings.find(id);
    if (it != m_strings.end())
    {
        return it->second;
    }
    else
    {
        throw std::runtime_error("String not found");
    }
}
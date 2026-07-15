#include "locale/string_manager.hpp"
#include <stdexcept>

locale::StringManager::StringManager()
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
        {StringId::TooltipEditorLayerModeNonActiveTransparent, L"Transparent mode"},
        {StringId::TooltipEditorLayerModeSingleLayer, L"Single layer mode"},
        {StringId::TooltipEditorLayerModeSingleImage, L"Single image mode"},
        {StringId::TooltipEditorChunkModeFixedSize, L"Fixed size chunk mode"},
        {StringId::TooltipEditorChunkModeFree, L"Free chunk mode"},        

        {StringId::MenuFile, L"File"},
        {StringId::MenuEdit, L"Edit"},
    };
}

const std::wstring& locale::StringManager::Get(StringId id) const
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
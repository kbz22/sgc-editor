#include "action/change_brush_erase_mode_action.hpp"
#include "program/program.hpp"

#include <stdexcept>

action::ChangeBrushEraseModeAction::ChangeBrushEraseModeAction(editor_tools::EraserMode eraserMode) :
    m_eraserMode(eraserMode)
{
    switch(eraserMode) {

        case editor_tools::EraserMode::None:
            throw std::invalid_argument("EraserMode::None is not a valid mode for ChangeBrushEraseModeAction");

        case editor_tools::EraserMode::ClearTile:
            m_actionType = ActionType::EraseModeClearTile;
            m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorEraseModeClearTile;
            m_actionDescription.nameStringId = locale::StringId::NameEraserModeClearTile;
            m_actionDescription.imageIndex = 30;
            m_actionDescription.toolbarOrder = 1900;
            m_actionDescription.menuOrder = 850;
            /* m_actionDescription.shortcuts = {
                { win32_program::ShortcutModifier::None, win32_program::ShortcutKey::Delete },
                { win32_program::ShortcutModifier::None, 'D' }
            }; */
            break;

        case editor_tools::EraserMode::DeleteChunk:
            m_actionType = ActionType::EraseModeDeleteChunk;
            m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorEraseModeDeleteChunk;
            m_actionDescription.nameStringId = locale::StringId::NameEraserModeDeleteChunk;
            m_actionDescription.imageIndex = 31;
            m_actionDescription.toolbarOrder = 1910;
            m_actionDescription.menuOrder = 860;
            /* m_actionDescription.shortcuts = {
                { win32_program::ShortcutModifier::Ctrl, 'D' },
                { win32_program::ShortcutModifier::Shift, win32_program::ShortcutKey::Delete }
            }; */
            break;        
    }

    m_enabled = true;
    m_checked = false;

    m_actionDescription.checkGroupItem = true;
    m_actionDescription.groupId = GroupId::EraseMode;
    m_actionDescription.menuId = MenuId::Edit;
    m_actionDescription.shortcutContext = win32_program::ShortcutContext::MapEditor;    
}

void action::ChangeBrushEraseModeAction::Execute(program::ProgramContext& context)
{
    auto currentEraserMode = context.GetSection<sections::MapSection>()->GetEraseMode();

    if(m_eraserMode == currentEraserMode) {
        context.UpdateBrushEraseMode(editor_tools::EraserMode::None);
    }
    else {
        context.UpdateBrushEraseMode(m_eraserMode);
    }
}
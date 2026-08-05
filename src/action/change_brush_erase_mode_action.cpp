#include "action/change_brush_erase_mode_action.hpp"
#include "program/program.hpp"
#include "program/editor_update.hpp"
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
            m_actionDescription.menuOrder = 650;
            break;

        case editor_tools::EraserMode::DeleteChunk:
            m_actionType = ActionType::EraseModeDeleteChunk;
            m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorEraseModeDeleteChunk;
            m_actionDescription.nameStringId = locale::StringId::NameEraserModeDeleteChunk;
            m_actionDescription.imageIndex = 31;
            m_actionDescription.toolbarOrder = 2000;
            m_actionDescription.menuOrder = 660;
            break;        
    }

    m_enabled = true;
    m_checked = false;

    m_actionDescription.checkGroupItem = true;
    m_actionDescription.groupId = GroupId::EraseMode;
    m_actionDescription.menuId = MenuId::Edit;    
}

void action::ChangeBrushEraseModeAction::Execute(program::ProgramContext& context)
{
    auto currentEraserMode = context.mapSection->GetEraseMode();

    if(m_eraserMode == currentEraserMode) {
        program::UpdateBrushEraseMode(editor_tools::EraserMode::None);
    }
    else {
        program::UpdateBrushEraseMode(m_eraserMode);
    }
}
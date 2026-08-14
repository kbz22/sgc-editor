#include "action/change_brush_mode_action.hpp"
#include "program/program.hpp"
#include "program/editor_update.hpp"

action::ChangeBrushModeAction::ChangeBrushModeAction(editor_tools::PaintMode brushMode) :
    m_brushMode(brushMode)
{
    switch(m_brushMode) {

        case editor_tools::PaintMode::Brush:
            m_actionType = ActionType::PaintModeBrush;
            m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorPaintModeBrush;
            m_actionDescription.nameStringId = locale::StringId::NamePaintModeBrush;
            m_actionDescription.imageIndex = 19;
            m_actionDescription.toolbarOrder = 1500;
            m_actionDescription.menuOrder = 800;
            break;

        case editor_tools::PaintMode::Rectangle:
            m_actionType = ActionType::PaintModeRectangle;
            m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorPaintModeRectangle;
            m_actionDescription.nameStringId = locale::StringId::NamePaintModeRectangle;
            m_actionDescription.imageIndex = 20;
            m_actionDescription.toolbarOrder = 1600;
            m_actionDescription.menuOrder = 810;
            break;

        case editor_tools::PaintMode::Fill:
            m_actionType = ActionType::PaintModeFill;
            m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorPaintModeFill;
            m_actionDescription.nameStringId = locale::StringId::NamePaintModeFill;
            m_actionDescription.imageIndex = 22;
            m_actionDescription.toolbarOrder = 1700;
            m_actionDescription.menuOrder = 820;
            break;

        case editor_tools::PaintMode::Select:
            m_actionType = ActionType::PaintModeSelect;
            m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorPaintModeSelect;
            m_actionDescription.nameStringId = locale::StringId::NamePaintModeSelect;
            m_actionDescription.imageIndex = 24;
            m_actionDescription.toolbarOrder = 1800;
            m_actionDescription.menuOrder = 830;
            break;
    }

    m_enabled = true;
    m_checked = false;

    m_actionDescription.checkGroupItem = true;
    m_actionDescription.groupId = GroupId::BrushMode;
    m_actionDescription.menuId = MenuId::Edit;
}

void action::ChangeBrushModeAction::Execute([[maybe_unused]] program::ProgramContext& context)
{    
    auto currentSelectionMode = context.mapSection->GetSelectionMode();    

    if(m_brushMode != editor_tools::PaintMode::Select) {
        context.mapSection->ResetSelection();
    }

    program::UpdateBrushMode(m_brushMode, context);
    program::UpdateEditorSelectionMode(currentSelectionMode, context);
    program::UpdateEditorSelectionTools(context);
}
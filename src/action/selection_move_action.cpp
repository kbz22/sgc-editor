#include "action/selection_move_action.hpp"
#include "program/program.hpp"

action::SelectionMoveAction::SelectionMoveAction()
{
    m_actionType = ActionType::SelectionMove;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = 32;
    m_actionDescription.toolbarOrder = 2100;
    m_actionDescription.menuOrder = 700;
    m_actionDescription.checkGroupItem = true;
    m_actionDescription.groupId = GroupId::SelectionTools;
    m_actionDescription.menuId = MenuId::Edit;
    m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorSelectionMove;
    m_actionDescription.nameStringId = locale::StringId::NameEditorSelectionMove;
    m_actionDescription.shortcutContext = program::ShortcutContext::MapEditorSelection;
    m_actionDescription.shortcuts = {
        {program::ShortcutModifier::Ctrl, 'M'}
    };
}

void action::SelectionMoveAction::Execute(program::ProgramContext &context)
{
    bool isMovingSelection = context.mapSection->GetSelectionMoveMode();
    context.mapSection->SetSelectionMoveMode(!isMovingSelection);
}
    
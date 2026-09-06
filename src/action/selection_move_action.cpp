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
    m_actionDescription.shortcutContext = win32_program::ShortcutContext::MapEditorSelection;
    m_actionDescription.shortcuts = {
        {win32_program::ShortcutModifier::Ctrl, 'M'}
    };
}

void action::SelectionMoveAction::Execute(program::ProgramContext &context)
{
    bool canMove = context.mapSection->GetSelectionMoveMode();
    context.mapSection->SetSelectionMoveMode(!canMove);

    // I can't figure out what unchecks the button. When you click certain other toolbar buttons this one unchecks.
    // This makes it stay checked
    SetChecked(!canMove);
}
    
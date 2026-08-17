#include "action/selection_cut_action.hpp"
#include "program/program.hpp"

action::SelectionCutAction::SelectionCutAction()
{
    m_actionType = ActionType::SelectionCut;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = 26;
    m_actionDescription.toolbarOrder = 2000;
    m_actionDescription.menuOrder = 300;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::SelectionTools;
    m_actionDescription.menuId = MenuId::Edit;
    m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorSelectionCut;
    m_actionDescription.nameStringId = locale::StringId::NameEditorSelectionCut;
    m_actionDescription.shortcutContext = program::ShortcutContext::MapEditorSelection;
    m_actionDescription.shortcuts = {
        {program::ShortcutModifier::Ctrl, 'X'}
    };
}

void action::SelectionCutAction::Execute(program::ProgramContext &context)
{
    auto &actionManager = *context.actionManager;

    actionManager.Find(action::ActionType::SelectionCopy)->Execute(context);
    actionManager.Find(action::ActionType::SelectionClear)->Execute(context);
}
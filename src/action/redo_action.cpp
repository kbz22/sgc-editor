#include "action/redo_action.hpp"
#include "program/program.hpp"

action::RedoAction::RedoAction()
{
    m_actionType = ActionType::Redo;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = 12;
    m_actionDescription.toolbarOrder = 500;
    m_actionDescription.menuOrder = 200;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::UndoRedo;
    m_actionDescription.menuId = MenuId::Edit;
    m_actionDescription.tooltipStringId = locale::StringId::TooltipEditRedo;
    m_actionDescription.nameStringId = locale::StringId::NameRedo;
}

void action::RedoAction::Execute(program::ProgramContext& context)
{
    if(context.commandManager != nullptr) {
        context.commandManager->Redo();
    }

    context.mapSection->Update();
}
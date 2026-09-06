#include "action/undo_action.hpp"
#include "program/program.hpp"

action::UndoAction::UndoAction()
{
    m_actionType = ActionType::Undo;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = 11;
    m_actionDescription.toolbarOrder = 400;
    m_actionDescription.menuOrder = 100;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::UndoRedo;
    m_actionDescription.menuId = MenuId::Edit;
    m_actionDescription.tooltipStringId = locale::StringId::TooltipEditUndo;
    m_actionDescription.nameStringId = locale::StringId::NameUndo;
    m_actionDescription.shortcutContext = win32_program::ShortcutContext::MapEditor;
    m_actionDescription.shortcuts = {
        {win32_program::ShortcutModifier::Ctrl, 'Z'}
    };
}

void action::UndoAction::Execute(program::ProgramContext& context)
{
    auto selectedDocument = context.fileManager->GetActiveDocument();
    if(selectedDocument != nullptr) {
        selectedDocument->GetCommandManager()->Undo();
    }

    context.mapSection->Update();
}
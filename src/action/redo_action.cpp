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
    m_actionDescription.shortcutContext = win32_program::ShortcutContext::MapEditor;
    m_actionDescription.shortcuts = {
        {win32_program::ShortcutModifier::Ctrl, 'Y'},
        {win32_program::ShortcutModifier::Ctrl | win32_program::ShortcutModifier::Shift, 'Z'}
    };
}

void action::RedoAction::Execute(program::ProgramContext& context)
{
    auto selectedDocument = context.GetManager<file::FileManager>()->GetActiveDocument();
    if(selectedDocument != nullptr) {
        selectedDocument->GetCommandManager()->Redo();
    }

    context.GetSection<sections::MapSection>()->Update();
}
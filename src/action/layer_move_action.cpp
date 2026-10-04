#include "action/layer_move_action.hpp"
#include "program/program.hpp"

#include "command/layer_move_command.hpp"

action::LayerMoveAction::LayerMoveAction(int moveCount) :
    m_moveCount(moveCount)
{    
    m_enabled = true;
    m_checked = false;

    switch(moveCount) 
    {
        case -1:
        {
            m_actionType = ActionType::MoveLayerDown;
            m_actionDescription.imageIndex = 9;
            m_actionDescription.toolbarOrder = 800;
            m_actionDescription.menuOrder = 400;
            m_actionDescription.tooltipStringId = locale::StringId::TooltipLayerMoveDown;
            m_actionDescription.nameStringId = locale::StringId::NameMoveLayerDown;
            m_actionDescription.shortcuts = {
                { win32_program::ShortcutModifier::Shift, win32_program::ShortcutKey::PageUp }
            };
            break;
        }

        case 1:
        {
            m_actionType = ActionType::MoveLayerUp;
            m_actionDescription.imageIndex = 10;
            m_actionDescription.toolbarOrder = 810;
            m_actionDescription.menuOrder = 400;
            m_actionDescription.tooltipStringId = locale::StringId::TooltipLayerMoveUp;
            m_actionDescription.nameStringId = locale::StringId::NameMoveLayerUp;
            m_actionDescription.shortcuts = {
                { win32_program::ShortcutModifier::Shift, win32_program::ShortcutKey::PageDown }
            };
            break;
        }

        default:
        {
            throw std::logic_error("LayerMoveAction only supports moveCount of 1 or -1");
            break;
        }
    }    
    
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::LayerManagement;
    m_actionDescription.menuId = MenuId::Map;
    m_actionDescription.shortcutContext = win32_program::ShortcutContext::MapEditor;
}

void action::LayerMoveAction::Execute(program::ProgramContext& context)
{
    auto selectedDocument = context.GetManager<file::FileManager>()->GetActiveDocument();
    if(selectedDocument != nullptr) {
        selectedDocument->GetCommandManager()->Execute(
            std::make_unique<command::LayerMoveCommand>(m_moveCount)
        );

        context.UpdateEditorLayerMode(context.GetEditorLayerMode());
    }
}
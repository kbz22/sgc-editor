#include "action/layer_move_action.hpp"
#include "program/program.hpp"
#include "program/editor_update.hpp"
#include "command/layer_move_command.hpp"

action::LayerMoveAction::LayerMoveAction(int moveCount) :
    m_moveCount(moveCount)    
{
    m_actionType = (moveCount < 0) ? ActionType::MoveLayerDown : ActionType::MoveLayerUp;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = (moveCount < 0) ? 9 : 10;
    m_actionDescription.toolbarOrder = 800;
    m_actionDescription.menuOrder = 400;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::LayerManagement;
    m_actionDescription.menuId = MenuId::Map;
    m_actionDescription.tooltipStringId = (moveCount < 0) ? locale::StringId::TooltipLayerMoveDown : locale::StringId::TooltipLayerMoveUp;
    m_actionDescription.nameStringId = (moveCount < 0) ? locale::StringId::NameMoveLayerDown : locale::StringId::NameMoveLayerUp;
}

void action::LayerMoveAction::Execute(program::ProgramContext& context)
{
    if(context.layerManager != nullptr) {
        context.commandManager->Execute(
            std::make_unique<command::LayerMoveCommand>(m_moveCount)
        );

        program::UpdateEditorLayerMode(context.editorLayerMode);
    }
}
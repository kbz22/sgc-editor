#include "action/layer_remove_action.hpp"
#include "program/program.hpp"
#include "program/editor_update.hpp"
#include "command/layer_remove_command.hpp"

action::LayerRemoveAction::LayerRemoveAction()
{
    m_actionType = ActionType::RemoveLayer;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = 8;
    m_actionDescription.toolbarOrder = 700;
    m_actionDescription.menuOrder = 200;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::LayerManagement;
    m_actionDescription.menuId = MenuId::Map;
    m_actionDescription.tooltipStringId = locale::StringId::TooltipLayerRemove;
    m_actionDescription.nameStringId = locale::StringId::NameRemoveLayer;
}

void action::LayerRemoveAction::Execute(program::ProgramContext& context)
{
    if(context.layerManager != nullptr) {
        context.commandManager->Execute(
            std::make_unique<command::LayerRemoveCommand>()
        );

        program::UpdateEditorLayerMode(context.editorLayerMode);
    }
}
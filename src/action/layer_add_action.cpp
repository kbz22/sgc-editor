#include "action/layer_add_action.hpp"
#include "program/program.hpp"

#include "command/layer_add_command.hpp"

action::LayerAddAction::LayerAddAction()
{
    m_actionType = ActionType::AddLayer;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = 7;
    m_actionDescription.toolbarOrder = 600;
    m_actionDescription.menuOrder = 100;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::LayerManagement;
    m_actionDescription.menuId = MenuId::Map;
    m_actionDescription.tooltipStringId = locale::StringId::TooltipLayerAdd;
    m_actionDescription.nameStringId = locale::StringId::NameAddLayer;
}

void action::LayerAddAction::Execute(program::ProgramContext& context)
{    
    auto selectedDocument = context.GetManager<file::FileManager>()->GetActiveDocument();
    if(selectedDocument != nullptr) {
        
        selectedDocument->GetCommandManager()->Execute(
            std::make_unique<command::LayerAddCommand>()
        );

        context.UpdateEditorLayerMode(context.GetEditorLayerMode());
    }
}
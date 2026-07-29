#include "action/change_layer_mode_action.hpp"
#include "program/program.hpp"
#include "program/editor_update.hpp"

action::ChangeLayerModeAction::ChangeLayerModeAction(program::EditorLayerMode layerMode) :
    m_layerMode(layerMode)
{
    switch(m_layerMode) {

        case program::EditorLayerMode::MultiLayer:
            m_actionType = ActionType::LayerModeMultilayer;
            m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorLayerModeMultilayer;
            m_actionDescription.nameStringId = locale::StringId::NameEditorLayerModeMultilayer;
            m_actionDescription.imageIndex = 4;
            m_actionDescription.toolbarOrder = 1000;
            m_actionDescription.menuOrder = 100;
            break;

        case program::EditorLayerMode::SingleLayer:
            m_actionType = ActionType::LayerModeSingleLayer;
            m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorLayerModeSingleLayer;
            m_actionDescription.nameStringId = locale::StringId::NameEditorLayerModeSingleLayer;
            m_actionDescription.imageIndex = 5;
            m_actionDescription.toolbarOrder = 1100;
            m_actionDescription.menuOrder = 101;
            break;
        
        case program::EditorLayerMode::SingleImage:
            m_actionType = ActionType::LayerModeSingleImage;
            m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorLayerModeSingleImage;
            m_actionDescription.nameStringId = locale::StringId::NameEditorLayerModeSingleImage;
            m_actionDescription.imageIndex = 6;
            m_actionDescription.toolbarOrder = 1200;
            m_actionDescription.menuOrder = 102;
            break;
    }
    
    m_enabled = true;
    m_checked = false;
    
    m_actionDescription.checkGroupItem = true;
    m_actionDescription.groupId = GroupId::EditorLayerMode;
    m_actionDescription.menuId = MenuId::View;
}

void action::ChangeLayerModeAction::Execute(program::ProgramContext& context)
{
    context.editorLayerMode = m_layerMode;
    program::UpdateEditorLayerMode(context.editorLayerMode);
}
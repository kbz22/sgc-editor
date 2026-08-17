#include "action/change_selection_mode_action.hpp"
#include "program/program.hpp"
#include "program/editor_update.hpp"

action::ChangeSelectionModeAction::ChangeSelectionModeAction(editor_tools::SelectionMode selectionMode) :
    m_selectionMode{selectionMode}
{
    switch(m_selectionMode) {

        case editor_tools::SelectionMode::SingleLayer:
            m_actionType = ActionType::SelectSingleLayerMode;
            m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorSelectModeSingleLayer;
            m_actionDescription.nameStringId = locale::StringId::NameEditorSelectionModeSingleLayer;
            m_actionDescription.imageIndex = 33;
            m_actionDescription.toolbarOrder = 2500;
            m_actionDescription.menuOrder = 900;
            break;

        case editor_tools::SelectionMode::AllLayers:
            m_actionType = ActionType::SelectAllLayersMode;
            m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorSelectModeAllLayers;
            m_actionDescription.nameStringId = locale::StringId::NameEditorSelectionModeAllLayers;
            m_actionDescription.imageIndex = 34;
            m_actionDescription.toolbarOrder = 2510;
            m_actionDescription.menuOrder = 910;
            break;

        case editor_tools::SelectionMode::VisibleLayers:
            m_actionType = ActionType::SelectVisibleLayersMode;
            m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorSelectModeVisibleLayers;
            m_actionDescription.nameStringId = locale::StringId::NameEditorSelectionModeVisibleLayers;
            m_actionDescription.imageIndex = 35;
            m_actionDescription.toolbarOrder = 2520;
            m_actionDescription.menuOrder = 920;
            break;

    }

    m_enabled = true;
    m_checked = false;
    
    m_actionDescription.checkGroupItem = true;
    m_actionDescription.groupId = GroupId::SelectionToolsMode;
    m_actionDescription.menuId = MenuId::Edit;
}

void action::ChangeSelectionModeAction::Execute(program::ProgramContext& programContext)
{
    program::UpdateEditorSelectionMode(m_selectionMode, programContext);
}
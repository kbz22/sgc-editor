#include "action/change_grid_mode_action.hpp"
#include "program/program.hpp"
#include "program/editor_update.hpp"
#include <stdexcept>

action::ChangeGridModeAction::ChangeGridModeAction(program::EditorGridMode newGridMode) :
    m_newGridMode(newGridMode)
{
    switch(m_newGridMode){
        
        case program::EditorGridMode::NoGrid:
            throw std::invalid_argument("EditorGridMode::NoGrid is not a valid mode for ChangeGridModeAction");

        case program::EditorGridMode::TileGrid:
            m_actionType = ActionType::GridModeTile;
            m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorTileLineGrid;
            m_actionDescription.nameStringId = locale::StringId::NameEditorTileLineGrid;
            m_actionDescription.imageIndex = 28;
            m_actionDescription.toolbarOrder = 1110;
            m_actionDescription.menuOrder = 700;
            break;

        case program::EditorGridMode::ChunkGrid:
            m_actionType = ActionType::GridModeChunk;
            m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorChunkLineGrid;
            m_actionDescription.nameStringId = locale::StringId::NameEditorChunkLineGrid;
            m_actionDescription.imageIndex = 29;
            m_actionDescription.toolbarOrder = 1120;
            m_actionDescription.menuOrder = 710;
            break;

    }

    m_enabled = true;
    m_checked = false;

    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::GridMode;
    m_actionDescription.menuId = MenuId::View;    
}

void action::ChangeGridModeAction::Execute(program::ProgramContext& context)
{
    program::UpdateEditorGridMode(m_newGridMode);
}
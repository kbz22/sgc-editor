#include "action/change_chunk_mode_action.hpp"
#include "program/editor_update.hpp"
#include "program/program.hpp"

action::ChangeChunkModeAction::ChangeChunkModeAction(program::EditorChunkMode chunkMode) :
    m_chunkMode(chunkMode)
{
    switch(m_chunkMode) {

        case program::EditorChunkMode::FixedChunks:
            m_actionType = ActionType::ChunkModeFixedSize;
            m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorChunkModeFixedSize;
            m_actionDescription.nameStringId = locale::StringId::NameEditorChunkModeFixedSize;
            m_actionDescription.imageIndex = 16;
            m_actionDescription.toolbarOrder = 1300;
            m_actionDescription.menuOrder = 501;
            break;

        case program::EditorChunkMode::DynamicChunks:
            m_actionType = ActionType::ChunkModeFree;
            m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorChunkModeFree;
            m_actionDescription.nameStringId = locale::StringId::NameEditorChunkModeFree;
            m_actionDescription.imageIndex = 15;
            m_actionDescription.toolbarOrder = 1400;
            m_actionDescription.menuOrder = 502;
            break;        
    }
    
    m_enabled = true;
    m_checked = false;    
    
    m_actionDescription.checkGroupItem = true;
    m_actionDescription.groupId = GroupId::EditorChunkMode;
    m_actionDescription.menuId = MenuId::Map;
}

void action::ChangeChunkModeAction::Execute(program::ProgramContext& context)
{
    context.editorChunkMode = m_chunkMode;    
    program::UpdateEditorChunkMode(context.editorChunkMode);
}
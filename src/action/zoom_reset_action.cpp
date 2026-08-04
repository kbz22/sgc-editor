#include "action/zoom_reset_action.hpp"
#include "program/program.hpp"

action::ZoomResetAction::ZoomResetAction()
{
    m_actionType = ActionType::ResetZoom;
    m_enabled = true;
    m_checked = false;    
    
    m_actionDescription.imageIndex = 17;
    m_actionDescription.toolbarOrder = 1900;
    m_actionDescription.menuOrder = -1;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::Zoom;
    m_actionDescription.menuId = MenuId::None;    
    m_actionDescription.tooltipStringId = locale::StringId::TooltipResetZoom;
    m_actionDescription.nameStringId = locale::StringId::TextMissing;
}

void action::ZoomResetAction::Execute(program::ProgramContext& context)
{
    context.mapSection->ExecuteZoom(
        context.mapSection->GetScreenCenterWorldPosition(),
        1.0f
    );
    context.mapSection->Update();
}
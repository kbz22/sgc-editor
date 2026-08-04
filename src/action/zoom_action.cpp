#include "action/zoom_action.hpp"
#include "program/program.hpp"

action::ZoomAction::ZoomAction(float zoomFactor)
    : m_zoomFactor(zoomFactor)
{
    m_enabled = true;
    m_checked = false;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::Zoom;
    
    if(zoomFactor > 1.0f)
    {
        m_actionType = ActionType::ZoomOut;
        m_actionDescription.imageIndex = -1;
        m_actionDescription.toolbarOrder = -1;
        m_actionDescription.menuOrder = 500;
        m_actionDescription.menuId = MenuId::View;
        m_actionDescription.tooltipStringId = std::nullopt;
        m_actionDescription.nameStringId = locale::StringId::NameZoomOut;
    }
    else
    {
        m_actionType = ActionType::ZoomIn;
        m_actionDescription.imageIndex = -1;
        m_actionDescription.toolbarOrder = -1;
        m_actionDescription.menuOrder = 400;
        m_actionDescription.menuId = MenuId::View;
        m_actionDescription.tooltipStringId = std::nullopt;
        m_actionDescription.nameStringId = locale::StringId::NameZoomIn;
    }

}

void action::ZoomAction::Execute(program::ProgramContext& context)
{
    context.mapSection->ExecuteZoom(
        context.mapSection->GetScreenCenterWorldPosition(),
        context.mapSection->GetZoom() * m_zoomFactor
    );
    context.mapSection->Update();
}
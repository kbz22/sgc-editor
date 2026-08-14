#include "action/zoom_select_action.hpp"
#include "program/program.hpp"
#include "action/action_types.hpp"

action::ZoomSelectAction::ZoomSelectAction()
{
    m_actionType = ActionType::SelectZoom;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = -1;
    m_actionDescription.toolbarOrder = 2600;
    m_actionDescription.menuOrder = -1;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::Zoom;
    m_actionDescription.menuId = MenuId::None;
    m_actionDescription.tooltipStringId = locale::StringId::TooltipEditorSetZoom;
    m_actionDescription.nameStringId = locale::StringId::TextMissing;
}

void action::ZoomSelectAction::BuildWidget(HWND parent, program::ProgramContext& context)
{
    m_zoomComboBox = std::make_unique<win32_models::ZoomComboBox>(
        parent,
        context.mainWindowContext->hInstance,
        static_cast<int>(m_actionType)
    );

    m_zoomComboBox->RegisterOnUpdateCallback([this, &context]() {
        this->Execute(context);
    });

    context.mapSection->RegisterOnZoomChangedCallback([this](float zoom) {
        m_zoomComboBox->SetZoomLevel(zoom);
    });

}

int action::ZoomSelectAction::GetControlWidth() const
{
    return m_zoomComboBox->GetWidth();
}

HWND action::ZoomSelectAction::GetHWND() const
{
    return m_zoomComboBox->GetHWND();
}

win32_models::IWidget* action::ZoomSelectAction::GetWidget() const
{
    return m_zoomComboBox.get();
}

void action::ZoomSelectAction::Execute(program::ProgramContext& context)
{
    float zoomLevel = m_zoomComboBox->GetZoomLevel();

    context.mapSection->ExecuteZoom(
        context.mapSection->GetScreenCenterWorldPosition(),
        zoomLevel
    );
    context.mapSection->Update();
}

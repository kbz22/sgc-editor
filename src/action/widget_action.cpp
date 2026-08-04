#include "action/widget_action.hpp"

void action::WidgetAction::SetControlHandle(HWND hwnd)
{
    m_controlHandle = hwnd;
}

bool action::WidgetAction::IsWidget() const
{
    return true;
}

HWND action::WidgetAction::GetHWND() const
{
    return m_controlHandle;
}
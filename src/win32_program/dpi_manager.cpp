#include "win32_program/dpi_manager.hpp"

win32_program::DpiManager::DpiManager(HWND hwnd)
{
    m_dpi = GetDpiForWindow(hwnd);
}

void win32_program::DpiManager::SetDpi(UINT dpi)
{
    m_dpi = dpi;
}

UINT win32_program::DpiManager::GetDpi() const
{
    return m_dpi;
}

double win32_program::DpiManager::GetScale() const
{
    return static_cast<double>(m_dpi) / static_cast<double>(USER_DEFAULT_SCREEN_DPI);
}

int win32_program::DpiManager::Scale(int value) const
{
    return MulDiv(value, m_dpi, USER_DEFAULT_SCREEN_DPI); 
}
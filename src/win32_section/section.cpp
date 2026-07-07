#include "win32_section/section.hpp"

win32_section::Section::Section(LPCWSTR name, win32_program::ControlId id, win32_program::Win32Context& context)
{    
    m_hwnd = CreateWindowEx(
        0, L"SectionWindow", name,
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS,
        0,0,0,0,
        context.hMainWindow, (HMENU)id, context.hInstance, nullptr
    );
    m_parentHwnd = context.hMainWindow;
}

win32_section::Section::~Section()
{
    if (m_hwnd)
    {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
}

HWND win32_section::Section::GetHwnd() const
{
    return m_hwnd;
}
    
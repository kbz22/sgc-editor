#include "sections/section.hpp"

sections::Section::Section(LPCWSTR name, win32_program::ControlId id, win32_program::Win32Context &context)
{    
    m_hwnd = CreateWindowEx(
        0, L"SectionWindow", name,
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS,
        0,0,0,0,
        context.hMainWindow, (HMENU)id, context.hInstance, nullptr
    );
    m_parentHwnd = context.hMainWindow;
}

sections::Section::Section()
{
    m_hwnd = HWND();
    m_parentHwnd = HWND();
}

sections::Section::~Section()
{
    if (m_hwnd)
    {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
}

HWND sections::Section::GetHwnd() const
{
    return m_hwnd;
}

void sections::Section::SetHwnd(HWND hwnd, HWND parentHwnd)
{
    m_hwnd = hwnd;
    m_parentHwnd = parentHwnd;
}
    
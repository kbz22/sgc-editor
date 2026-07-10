#include "sections/section.hpp"
#include <commctrl.h>

namespace {
    constexpr UINT_PTR kTilesetSubclassId = 0x53474331;
}

sections::Section::Section(LPCWSTR name, win32_program::ControlId id, win32_program::MainWindowContext &context)
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

        // RemoveWindowSubclass(m_sectionWindow, StaticPaneProc, kTilesetSubclassId);
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

LRESULT CALLBACK sections::Section::StaticPaneProc([[maybe_unused]] HWND hwnd, [[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wparam, [[maybe_unused]] LPARAM lparam, [[maybe_unused]] UINT_PTR id, [[maybe_unused]] DWORD_PTR data) 
{
    auto* self = reinterpret_cast<Section*>(data);

    if (self)
    {
        return self->HandleMessages(
            hwnd,
            msg,
            wparam,
            lparam);
    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

LRESULT sections::Section::HandleMessages([[maybe_unused]] HWND hwnd, [[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wparam, [[maybe_unused]] LPARAM lparam)
{
    return DefSubclassProc(hwnd, msg, wparam, lparam);
}
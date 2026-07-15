#include "sections/section.hpp"
#include <sgc/sdl/sdl_win32.hpp>
#include <commctrl.h>

namespace {
    constexpr UINT_PTR kTilesetSubclassId = 0x53474331;
}

bool sections::Section::m_registered = false;

sections::Section::Section(LPCWSTR name, win32_program::ControlId id, win32_program::MainWindowContext &context)
{    

    const wchar_t* className = L"SectionWindow";

    if(!m_registered) {
        WNDCLASSEX wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = DefaultSectionProc;
        wc.hInstance = context.hInstance;
        wc.lpszClassName = className;
        RegisterClassEx(&wc);

        m_registered = true;
    }

    m_hwnd = CreateWindowEx(
        0, className, name,
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
        RemoveWindowSubclass(m_hwnd, StaticPaneProc, kTilesetSubclassId);
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

LRESULT CALLBACK sections::Section::StaticPaneProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR id, DWORD_PTR data) 
{
    auto *self = reinterpret_cast<Section*>(data);

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

LRESULT sections::Section::HandleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

void sections::Section::AttachView(sgc_view::SgcView& view)
{
    m_hwnd = sgc::sdl::GetWin32HWND(view.GetSdlWindow());

    if (m_hwnd != nullptr) {
        SetWindowSubclass(m_hwnd, StaticPaneProc, kTilesetSubclassId, reinterpret_cast<DWORD_PTR>(this));
    }
    
}

void sections::Section::Redraw()
{
    if (m_hwnd != nullptr) {
        InvalidateRect(m_hwnd, nullptr, TRUE);
        UpdateWindow(m_hwnd);
    }
}

LRESULT CALLBACK sections::Section::DefaultSectionProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;

            GetClientRect(hwnd, &rc);
            FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW + 1));

            EndPaint(hwnd, &ps);
            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}
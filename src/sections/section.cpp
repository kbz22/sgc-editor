#include "sections/section.hpp"
#include "program/program.hpp"
#include <sgc/sdl/sdl_win32.hpp>
#include <commctrl.h>

namespace {
    constexpr UINT_PTR kTilesetSubclassId = 0x53474331;
}

bool sections::Section::m_registered = false;

sections::Section::Section(LPCWSTR name, win32_program::ControlId id, HWND parentHwnd, HINSTANCE hInstance)
{
    const wchar_t* className = L"SectionWindow";

    if(!m_registered) {
        WNDCLASSEX wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = DefaultSectionProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = className;
        RegisterClassEx(&wc);

        m_registered = true;
    }

    m_hwnd = CreateWindowEx(
        0, className, name,
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS,
        0,0,0,0,
        parentHwnd, (HMENU)id, hInstance, nullptr
    );    
    m_parentHwnd = parentHwnd;
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

// #include "debug.hpp"

LRESULT CALLBACK sections::Section::StaticPaneProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, [[maybe_unused]]UINT_PTR id, DWORD_PTR data) 
{
    auto *self = reinterpret_cast<Section*>(data);
    auto &programContext = program::GetProgramContext();

    if (self)
    {
        if(msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK || msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK)
        {
            SetFocus(hwnd);
            // programContext.activeSection = self;
            programContext.SetActiveSection(self);
            // debug::DebugLog(L"Active section set to: %p", self);
            
        }
        else if(msg == WM_KILLFOCUS)
        {
            if(programContext.GetActiveSection() == self) {
                programContext.SetActiveSection(nullptr);
                // debug::DebugLog(L"Active section cleared.");
            }
        }
        
        return self->HandleMessages(
            hwnd,
            msg,
            wparam,
            lparam
        );
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

void sections::Section::SetupSubclass(HWND hwnd, Section* section)
{
    if (hwnd != nullptr) {
        SetWindowSubclass(hwnd, StaticPaneProc, 0, reinterpret_cast<DWORD_PTR>(section));
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
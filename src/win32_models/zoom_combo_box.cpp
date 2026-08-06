#include "win32_models/zoom_combo_box.hpp"
#include <string>
#include <windowsx.h>
#include <commctrl.h>

win32_models::ZoomComboBox::ZoomComboBox(HWND parent, HINSTANCE hInstance, int id) :
    m_hwnd(nullptr),
    m_parent(parent),    
    m_zoom(1.0f)
{
    m_hwnd = CreateWindowEx(
        0,
        WC_COMBOBOX,
        L"",
        WS_CHILD | WS_VISIBLE |
        CBS_DROPDOWN |
        CBS_AUTOHSCROLL,
        0, 0, m_width, 200,
        parent,
        reinterpret_cast<HMENU>(id),
        hInstance,
        nullptr
    );

    for (auto preset : m_zoomLevelPresets)
    {
        wchar_t buffer[16];
        swprintf(buffer, 16, L"%.0f%%", preset * 100);
        SendMessage(m_hwnd, CB_ADDSTRING, 0, (LPARAM)buffer);
    }

    SetWindowSubclass(
        m_hwnd,
        ComboBoxStaticProc,
        0,
        reinterpret_cast<DWORD_PTR>(this)
    );
}

LRESULT CALLBACK win32_models::ZoomComboBox::ComboBoxStaticProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, [[maybe_unused]] UINT_PTR id, [[maybe_unused]] DWORD_PTR data)
{
    auto *self = reinterpret_cast<win32_models::ZoomComboBox*>(data);
    if (self)
    {
        return self->HandleMessage(hwnd, msg, wparam, lparam);
    }
    
    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

LRESULT win32_models::ZoomComboBox::HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
        case WM_COMMAND:
        {
            auto code = HIWORD(wparam);
            
            switch (code)
            {
                case CBN_SELCHANGE:
                {
                    int selectedIndex = SendMessage(hwnd, CB_GETCURSEL, 0, 0);
                    if (selectedIndex >= 0 && selectedIndex < static_cast<int>(ZoomLevelPresetsCount))
                    {
                        m_zoom = m_zoomLevelPresets[selectedIndex];
                    }
                }
                break;

                default:
                    break;
            }
            break;
        }

        default:
            break;
    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

float win32_models::ZoomComboBox::GetZoomLevel() const
{
    return m_zoom;
}

void win32_models::ZoomComboBox::SetZoomLevel(float zoomLevel)
{
    m_zoom = zoomLevel;
}

HWND win32_models::ZoomComboBox::GetHWND() const
{
    return m_hwnd;
}

int win32_models::ZoomComboBox::GetWidth() const
{
    return m_width;
}

void win32_models::ZoomComboBox::SetPosition(int x, int y)
{
    m_bounds.left = x;
    m_bounds.top = y;
    m_bounds.right = x + m_width;
    m_bounds.bottom = y + 24;
}

void win32_models::ZoomComboBox::Update()
{
    SetWindowPos(
        m_hwnd,
        nullptr,
        m_bounds.left,
        m_bounds.top,
        m_bounds.right - m_bounds.left,
        m_bounds.bottom - m_bounds.top,
        SWP_NOZORDER
    );
}
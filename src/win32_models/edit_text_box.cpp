#include "win32_models/edit_text_box.hpp"
#include <commctrl.h>

win32_models::EditTextBox::EditTextBox(HWND hwndParent, HINSTANCE hInstance) :
    m_hwnd(nullptr),
    m_visible(true)
{
    m_hwnd = CreateWindowExW(
        0,
        L"EDIT",
        L"", // initial text
        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        m_x, m_y, m_width, m_height,
        hwndParent,
        nullptr,
        hInstance,
        nullptr
    );

    SetWindowSubclass(
        m_hwnd,
        EditTextBoxStaticProc,
        0,
        reinterpret_cast<DWORD_PTR>(this)
    );

    ShowWindow(m_hwnd, SW_HIDE);
}

win32_models::EditTextBox::~EditTextBox()
{
    if (m_hwnd != nullptr)
    {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
}

LRESULT CALLBACK win32_models::EditTextBox::EditTextBoxStaticProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, [[maybe_unused]] UINT_PTR id, [[maybe_unused]] DWORD_PTR data)
{
    auto *self = reinterpret_cast<EditTextBox*>(data);
    if (self)
    {
        return self->HandleMessage(hwnd, msg, wparam, lparam);
    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

LRESULT win32_models::EditTextBox::HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
        case WM_PAINT:
        {
            if(!m_visible)
            {
                return 0;
            }
            break;
        }

        case WM_KEYDOWN:
        {
            if (wparam == VK_RETURN)
            {
                int length = GetWindowTextLengthW(m_hwnd);
                std::wstring text(length, L'\0');

                GetWindowTextW(
                    m_hwnd,
                    text.data(),
                    length + 1
                );

                m_onEditingFinished(text);

                m_visible = false;
                ShowWindow(m_hwnd, SW_HIDE);

                return 0;
            }

            if (wparam == VK_ESCAPE)
            {
                m_visible = false;
                ShowWindow(m_hwnd, SW_HIDE);

                return 0;
            }

            break;
        }

        default:
            break;
    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

void win32_models::EditTextBox::SetSize(int width, int height)
{
    m_width = width;
    m_height = height;    
}

void win32_models::EditTextBox::Update()
{
    if (m_hwnd != nullptr)
    {
        SetWindowPos(
            m_hwnd,
            nullptr,
            m_x, m_y, m_width, m_height,
            SWP_NOZORDER | SWP_NOACTIVATE
        );
    }
}

void win32_models::EditTextBox::StartEditing(int x, int y, std::wstring initialText, std::function<void(std::wstring)> onEditingFinished)
{
    m_x = x;
    m_y = y;

    SetWindowText(m_hwnd, initialText.c_str());

    m_onEditingFinished = onEditingFinished;
    m_visible = true;

    Update();

    ShowWindow(m_hwnd, SW_SHOW);    
}

HWND win32_models::EditTextBox::GetHwnd() const
{
    return m_hwnd;
}

int win32_models::EditTextBox::GetWidth() const
{
    return m_width;
}
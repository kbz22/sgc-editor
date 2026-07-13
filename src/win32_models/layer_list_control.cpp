#include "win32_models/layer_list_control.hpp"
#include "win32_helpers/load_bitmap.hpp"
#include <algorithm>
#include <windowsx.h>
#include "program/program.hpp"

bool win32_models::LayerListControl::m_isLayerListProcRegistered = false;

win32_models::LayerListControl::LayerListControl(HWND hwndParent, HINSTANCE hInstance, int x, int y, int width, int height)
{
    if(!m_isLayerListProcRegistered)
    {
        WNDCLASSEX wc = {};

        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = DefWindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = L"LayerListControl";
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = nullptr;

        RegisterClassEx(&wc);

        m_isLayerListProcRegistered = true;
    }

    m_hwnd = CreateWindowExW(
        0,
        L"LayerListControl",
        nullptr,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL,
        x, y, width, height,
        hwndParent,
        nullptr,
        hInstance,        
        nullptr
    );

    SetWindowSubclass(
        m_hwnd,
        LayerListStaticProc,
        0,
        reinterpret_cast<DWORD_PTR>(this)
    );
    
    m_imageList = ImageList_Create(24, 24, ILC_COLOR32, 10, 0);
    HBITMAP hBmp = win32_helpers::LoadPngWIC(L"./layerlist_icons.png");

    m_imageListDisabled = ImageList_Create(24, 24, ILC_COLOR32, 10, 0);
    HBITMAP hBmpDisabled = win32_helpers::LoadPngWIC(L"./layerlist_icons_disabled.png");

    ImageList_Add(m_imageList, hBmp, NULL);
    ImageList_Add(m_imageListDisabled, hBmpDisabled, NULL);

    ListItem item;
    for(int i=0; i<12; ++i) {
        item.name = L"Layer " + std::to_wstring(i + 1);
        m_layers.push_back(item);
    }    
}

win32_models::LayerListControl::~LayerListControl()
{
    if (m_hwnd != nullptr)
    {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
}

LRESULT CALLBACK win32_models::LayerListControl::LayerListStaticProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, [[maybe_unused]] UINT_PTR id, [[maybe_unused]] DWORD_PTR data)
{
    auto *self = reinterpret_cast<LayerListControl*>(data);
    if (self)
    {
        return self->HandleMessage(hwnd, msg, wparam, lparam);
    }
    
    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

void win32_models::LayerListControl::DrawEntry(HDC hdc, int index, const RECT& rect)
{
    if (index < 0 || index >= static_cast<int>(m_layers.size()))
    {
        return;
    }

    const auto& layer = m_layers[index];

    RECT row =
    {
        0,
        index * m_rowHeight - m_scrollOffsetPixels,
        rect.right - rect.left,
        (index + 1) * m_rowHeight - m_scrollOffsetPixels
    };

    HBRUSH brush;

    if (index == m_selectedLayerIndex)
    {
        brush = GetSysColorBrush(COLOR_HIGHLIGHT);
    }
    else if (index == m_hoveredLayerIndex)
    {
        brush = GetSysColorBrush(COLOR_BTNFACE);
    }
    else
    {
        brush = GetSysColorBrush(COLOR_WINDOW);
    }

    FillRect(
        hdc,
        &row,
        // m_selectedLayerIndex == index ? GetSysColorBrush(COLOR_HIGHLIGHT) : GetSysColorBrush(COLOR_WINDOW)
        brush
    );

    row.top += 2;

    ImageList_Draw(
        m_imageList,
        0,
        hdc,
        row.left + 4,
        row.top + 2,
        ILD_NORMAL
    );

    row.left += 36;
    row.top -= 4;
    
    SetBkMode(hdc, TRANSPARENT);
    DrawTextW(
        hdc,
        layer.name.c_str(),
        -1,
        &row,
        DT_LEFT | DT_VCENTER | DT_SINGLELINE
    );
}

LRESULT win32_models::LayerListControl::HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {        
        case WM_PAINT:
        {
            PAINTSTRUCT ps;

            HDC hdc = BeginPaint(hwnd, &ps);

            RECT clientRect;
            GetClientRect(hwnd, &clientRect);            

            FillRect(
                hdc,
                &ps.rcPaint,
                (HBRUSH)(COLOR_WINDOW + 1)
            );
            
            for(int index = 0; index < static_cast<int>(m_layers.size()); ++index){                
                DrawEntry(hdc, index, clientRect);
            }
            int contentHeight = static_cast<int>(m_layers.size()) * m_rowHeight;
            int visibleHeight = clientRect.bottom - clientRect.top;

            m_maxScroll = std::max(0, contentHeight - visibleHeight);

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_MOUSEWHEEL:
        {
            int delta = GET_WHEEL_DELTA_WPARAM(wparam);
            
            m_scrollOffsetPixels -= delta;

            m_scrollOffsetPixels =
                std::clamp(
                    m_scrollOffsetPixels,
                    0,
                    m_maxScroll);

            InvalidateRect(hwnd, nullptr, TRUE);
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            int x = GET_X_LPARAM(lparam);
            int y = GET_Y_LPARAM(lparam);

            auto prevHoveredIndex = m_hoveredLayerIndex;

            SetHoveredIndexAtPoint(x, y);

            if(prevHoveredIndex != m_hoveredLayerIndex && m_isMouseOverAnyEntry)
            {
                InvalidateRect(hwnd, nullptr, TRUE);
            }

            TRACKMOUSEEVENT tme{};

            tme.cbSize = sizeof(tme);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;

            TrackMouseEvent(&tme);
            
            return 0;
        }

        case WM_MOUSELEAVE:
        {
            m_hoveredLayerIndex = 0;
            m_isMouseOverAnyEntry = false;

            InvalidateRect(hwnd, nullptr, FALSE);

            return 0;
        }

        case WM_LBUTTONDOWN:
        {
            int y = GET_Y_LPARAM(lparam);
            int x = GET_X_LPARAM(lparam);

            SetHoveredIndexAtPoint(x, y);

            if (!m_isMouseOverAnyEntry)
            {
                return 0;
            }

            m_selectedLayerIndex = m_hoveredLayerIndex;

            InvalidateRect(hwnd, nullptr, FALSE);

            return 0;
        }

        default:
            return DefSubclassProc(hwnd, msg, wparam, lparam);
    }
}

void win32_models::LayerListControl::Resize(int x, int y, int width, int height)
{
    MoveWindow(
        m_hwnd,
        x,
        y,
        width,
        height,
        TRUE
    );
}

void win32_models::LayerListControl::Resize(int width, int height)
{
    Resize(0, 0, width, height);
}

void win32_models::LayerListControl::Refresh(program::LayerManager& layerManager)
{
    m_layers = layerManager.GetLayers();    
}

void win32_models::LayerListControl::SetSelectedLayer(size_t layerIndex)
{
    if (layerIndex < m_layers.size())
    {
        m_selectedLayerIndex = layerIndex;
    }
}

size_t win32_models::LayerListControl::GetSelectedLayer() const
{
    return m_selectedLayerIndex;
}

void win32_models::LayerListControl::SetHoveredIndexAtPoint([[maybe_unused]] int x, int y)
{
    int index = (y + m_scrollOffsetPixels) / m_rowHeight;    

    if (index < 0 || index >= static_cast<int>(m_layers.size()))
    {
        m_hoveredLayerIndex = 0;
        m_isMouseOverAnyEntry = false;

        return;
    }
    
    m_hoveredLayerIndex = index;
    m_isMouseOverAnyEntry = true;

    return;
}
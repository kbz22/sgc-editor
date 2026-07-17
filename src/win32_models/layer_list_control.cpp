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

    {
    COLORREF c = GetSysColor(COLOR_HIGHLIGHT);

    BYTE r = GetRValue(c);
    BYTE g = GetGValue(c);
    BYTE b = GetBValue(c);

    auto Lerp = [](BYTE a, BYTE b, float t)
    {
        return (BYTE)(a + (b - a) * t);
    };

    COLORREF hover =
        RGB(
            Lerp(r, 255, 0.25f),
            Lerp(g, 255, 0.25f),
            Lerp(b, 255, 0.25f));
            
    m_brushHighlightHover = CreateSolidBrush(hover);
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

    int deltaIconHeight = (m_rowHeight - 24) / 2;

    RECT eyeButtonRect =
    {
        4,
        row.top + deltaIconHeight,
        4 + 24,
        row.bottom - deltaIconHeight
    };

    HBRUSH brushRect;
    HBRUSH brushEyeButton;    

    if (index == m_selectedLayerIndex)
    {
        brushRect = GetSysColorBrush(COLOR_HIGHLIGHT);
        brushEyeButton = GetSysColorBrush(COLOR_HIGHLIGHT);

        if(m_mouseOver == MouseTarget::EyeButton && index == m_hoveredLayerIndex)
        {
            brushEyeButton = m_brushHighlightHover;
        }        
    }
    else switch (m_mouseOver)
    {
        case MouseTarget::Entry:
            if (index == m_hoveredLayerIndex)
            {
                brushRect = GetSysColorBrush(COLOR_BTNFACE);
            }
            else
            {
                brushRect = GetSysColorBrush(COLOR_WINDOW);
            }
            brushEyeButton = brushRect;
            break;

        case MouseTarget::EyeButton:
            if (index == m_hoveredLayerIndex)
            {
                brushEyeButton = GetSysColorBrush(COLOR_BTNFACE);
            }
            else
            {
                brushEyeButton = GetSysColorBrush(COLOR_WINDOW);
            }
            brushRect = GetSysColorBrush(COLOR_WINDOW);
            break;

        default:
            brushRect = GetSysColorBrush(COLOR_WINDOW);
            brushEyeButton = brushRect;
            break;
    }

    FillRect(
        hdc,
        &row,        
        brushRect
    );

    FillRect(
        hdc,
        &eyeButtonRect,        
        brushEyeButton
    );

    row.top += 2;
    brushEyeButton = GetSysColorBrush(COLOR_WINDOW);    

    ImageList_Draw(
        m_imageList,
        layer.visible ? m_imageListIndexOpen : m_imageListIndexClosed,
        hdc,
        eyeButtonRect.left,
        eyeButtonRect.top,
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
            UpdateScrollInfo();
            return 0;
        }

        case WM_VSCROLL:
        {
            int action = LOWORD(wparam);

            switch(action)
            {
                case SB_LINEUP:
                    m_scrollOffsetPixels -= m_rowHeight;
                    break;

                case SB_LINEDOWN:
                    m_scrollOffsetPixels += m_rowHeight;
                    break;

                case SB_THUMBPOSITION:
                case SB_THUMBTRACK:
                    m_scrollOffsetPixels = HIWORD(wparam);
                    break;
            }

            // ClampScroll();
            UpdateScrollInfo();

            InvalidateRect(hwnd,nullptr,FALSE);

            break;
        }

        case WM_MOUSEMOVE:
        {
            int x = GET_X_LPARAM(lparam);
            int y = GET_Y_LPARAM(lparam);

            auto prevHoveredIndex = m_hoveredLayerIndex;
            auto previousTarget = m_mouseOver;

            SetHoveredIndexAtPoint(x, y);

            if(prevHoveredIndex != m_hoveredLayerIndex || previousTarget != m_mouseOver)
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
            m_mouseOver = MouseTarget::None;

            InvalidateRect(hwnd, nullptr, FALSE);

            return 0;
        }

        case WM_LBUTTONDOWN:
        {
            int y = GET_Y_LPARAM(lparam);
            int x = GET_X_LPARAM(lparam);

            SetHoveredIndexAtPoint(x, y);

            switch (m_mouseOver)
            {
                case MouseTarget::Entry:                    
                    SetSelectedLayer(m_hoveredLayerIndex);
                    break;

                case MouseTarget::EyeButton:
                    if (m_hoveredLayerIndex < m_layers.size())
                    {
                        auto& layer = m_layers[m_hoveredLayerIndex];
                        layer.visible = !layer.visible;

                        if (m_layerVisibilityChangeCallback) {
                            m_layerVisibilityChangeCallback(m_hoveredLayerIndex, layer.visible);
                        }
                    }
                    break;

                default:
                    break;
            }

            InvalidateRect(hwnd, nullptr, FALSE);

            return 0;
        }

        default:
            break;
    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
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

    UpdateScrollInfo();
    InvalidateRect(m_hwnd, nullptr, TRUE);
}

void win32_models::LayerListControl::Redraw()
{
    InvalidateRect(m_hwnd, nullptr, TRUE);
}

void win32_models::LayerListControl::Resize(int width, int height)
{
    Resize(0, 0, width, height);
}

void win32_models::LayerListControl::Refresh(program::LayerManager& layerManager)
{    
    m_layers.clear();
    for(auto &layer : layerManager.GetLayers())
    {
        ListItem item;
        item.name = layer.name;
        item.visible = layer.visible;

        m_layers.push_back(item);
    }

    UpdateScrollInfo();
}

void win32_models::LayerListControl::SetSelectedLayer(size_t layerIndex)
{
    if (layerIndex < m_layers.size())
    {
        m_selectedLayerIndex = layerIndex;

        if (m_selectedLayerChangeCallback) {
            m_selectedLayerChangeCallback(layerIndex);
        }
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
        m_mouseOver = MouseTarget::None;

        return;
    }

    m_hoveredLayerIndex = index;

    if(x < 32 && x > 0) {        
        m_mouseOver = MouseTarget::EyeButton;
    }
    else {
        m_mouseOver = MouseTarget::Entry;
    }

    return;
}

void win32_models::LayerListControl::UpdateScrollInfo()
{
    RECT clientRect;
    GetClientRect(m_hwnd, &clientRect);

    SCROLLINFO si = {};
    si.cbSize = sizeof(si);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin = 0;
    si.nMax = static_cast<int>(m_layers.size() * m_rowHeight);
    si.nPage = clientRect.bottom - clientRect.top;
    si.nPos = m_scrollOffsetPixels;

    SetScrollInfo(
        m_hwnd,
        SB_VERT,
        &si,
        TRUE
    );
}

void win32_models::LayerListControl::SetHImageList(HIMAGELIST imageList, HIMAGELIST imageListDisabled, int indexOpen, int indexClosed)
{
    m_imageListIndexOpen = indexOpen;
    m_imageListIndexClosed = indexClosed;
    m_imageList = imageList;
    m_imageListDisabled = imageListDisabled;
}

void win32_models::LayerListControl::RegisterSelectedLayerChangeCallback(std::function<void(size_t)> callback)
{
    m_selectedLayerChangeCallback = callback;
}

void win32_models::LayerListControl::RegisterLayerVisibilityChangeCallback(std::function<void(size_t, bool)> callback)
{
    m_layerVisibilityChangeCallback = callback;
}
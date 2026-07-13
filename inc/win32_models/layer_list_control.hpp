#pragma once

#include "program/layer_manager.hpp"
#include "win32_program/win32_context.hpp"

#include <windows.h>
#include <string>
#include <CommCtrl.h>

namespace win32_models {

    using ListItem = program::LayerItem;

    enum class MouseTarget {
        None,
        Entry,
        EyeButton
    };

    class LayerListControl
    {
        private:
            HWND m_hwnd;
            HIMAGELIST m_imageList = nullptr;
            HIMAGELIST m_imageListDisabled = nullptr;
            std::vector<ListItem> m_layers;
            size_t m_selectedLayerIndex = 0;
            size_t m_hoveredLayerIndex = 0;
            MouseTarget m_mouseOver = MouseTarget::None;
            static bool m_isLayerListProcRegistered;
            HBRUSH m_brushHighlightHover = nullptr;

            int m_rowHeight = 32;
            int m_scrollOffsetPixels = 0;
            int m_maxScroll = 0;

            static LRESULT CALLBACK LayerListStaticProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR id, DWORD_PTR data);
            LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

            void DrawEntry(HDC hdc, int index, const RECT& rect);
            void SetHoveredIndexAtPoint(int x, int y);

        public:
            LayerListControl(HWND hwndParent, HINSTANCE hInstance, int x, int y, int width, int height);
            ~LayerListControl();

            void Resize(int x, int y, int width, int height);
            void Resize(int width, int height);
            void Refresh(program::LayerManager& layerManager);

            void SetSelectedLayer(size_t layerIndex);

            size_t GetSelectedLayer() const;        
    };

}
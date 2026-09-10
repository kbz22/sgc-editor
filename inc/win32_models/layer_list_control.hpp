#pragma once

#include "program/layer_manager.hpp"
#include "win32_models/edit_text_box.hpp"

#include <windows.h>
#include <string>
#include <functional>
#include <memory>
#include <commctrl.h>

namespace program {
    class ProgramContext;
}

namespace win32_models {

    struct ListItem
    {
        std::wstring name;
        int nameWidth = 0;
        bool visible = true;        
    };

    enum class MouseTarget {
        None,
        Entry,
        EyeButton,
        LayerName
    };

    class LayerListControl
    {
        private:
            HWND m_hwnd;
            HIMAGELIST m_imageList = nullptr;
            HIMAGELIST m_imageListDisabled = nullptr;
            int m_imageListIndexOpen = 0;
            int m_imageListIndexClosed = 1;
            std::vector<ListItem> m_layers;
            size_t m_selectedLayerIndex = 0;
            size_t m_hoveredLayerIndex = 0;
            MouseTarget m_mouseOver = MouseTarget::None;
            static bool m_isLayerListProcRegistered;
            HBRUSH m_brushHighlightHover = nullptr;
            HFONT m_hFont = nullptr;
            std::unique_ptr<win32_models::EditTextBox> m_editTextBox = nullptr;

            int m_rowHeight = 32;
            int m_labelRectXOffset = 34;
            int m_labelTextOffsetX = 4;
            int m_labelTextOffsetY = 8;
            int m_scrollOffsetPixels = 0;
            int m_maxScroll = 0;

            std::function<void(size_t)> m_selectedLayerChangeCallback = nullptr;
            std::function<void(size_t, bool)> m_layerVisibilityChangeCallback = nullptr;
            std::function<void(size_t, std::wstring)> m_layerNameChangeCallback = nullptr;

            static LRESULT CALLBACK LayerListStaticProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR id, DWORD_PTR data);
            LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

            void DrawEntry(HDC hdc, int index, const RECT& rect);
            void SetHoveredIndexAtPoint(int x, int y);
            void UpdateScrollInfo();
            void UpdateLayerName(std::wstring newName, size_t layerIndex);
            
        public:
            LayerListControl(HWND hwndParent, HINSTANCE hInstance, int x, int y, int width, int height);
            ~LayerListControl();

            void Resize(int x, int y, int width, int height);
            void Resize(int width, int height);
            void Refresh(program::ProgramContext& programContext);
            void Redraw();

            void SetSelectedLayer(size_t layerIndex);
            void SetHImageList(HIMAGELIST imageList, HIMAGELIST imageListDisabled, int indexOpen = 0, int indexClosed = 1);

            size_t GetSelectedLayer() const;
            HWND GetHwnd() const;

            void RegisterSelectedLayerChangeCallback(std::function<void(size_t)> callback);            
            void RegisterLayerVisibilityChangeCallback(std::function<void(size_t, bool)> callback);            
            void RegisterLayerNameChangeCallback(std::function<void(size_t, std::wstring)> callback);
    };

}
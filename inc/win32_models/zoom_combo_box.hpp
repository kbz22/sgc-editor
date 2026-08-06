#pragma once

#include "win32_models/iwidget.hpp"
#include <windows.h>
#include <functional>

namespace win32_models {

    class ZoomComboBox : public IWidget
    {
        private:
            HWND m_hwnd{nullptr};
            HWND m_parent{nullptr};
            float m_zoom = 1.0f;
            int m_width = 80;
            RECT m_bounds{0, 0, m_width, 24};
            constexpr static size_t ZoomLevelPresetsCount = 7;
            const float m_zoomLevelPresets[ZoomLevelPresetsCount] = {
                0.25f,
                0.5f,
                0.75f,
                1.0f,
                1.25f,
                1.5f,
                2.0f
            };            

            static LRESULT CALLBACK ComboBoxStaticProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR id, DWORD_PTR data);
            LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

        public:
            ZoomComboBox(HWND parent, HINSTANCE hInstance, int id);
            ~ZoomComboBox() = default;

            float GetZoomLevel() const;

            HWND GetHWND() const override;
            int GetWidth() const override;
            void SetPosition(int x, int y) override;
            void Update() override;

            void SetZoomLevel(float zoomLevel);
    };

}
#pragma once

#include "sections/section.hpp"
#include "win32_models/layer_list_control.hpp"

namespace sections {

    class LayersSection : public Section
    {
        private:
            std::unique_ptr<win32_models::LayerListControl> m_layerListControl = nullptr;
            static LRESULT CALLBACK LayerListProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

        public:
            LayersSection(win32_program::MainWindowContext& context);
            ~LayersSection();

            void Update() override;
            void HandleSectionResize() override;
    };

}
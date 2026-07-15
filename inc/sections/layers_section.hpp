#pragma once

#include <windows.h>
#include <functional>
#include "sections/section.hpp"
#include "win32_models/layer_list_control.hpp"

namespace program {
    struct ProgramContext;
}

namespace sections {

    class LayersSection : public Section
    {
        private:
            std::unique_ptr<win32_models::LayerListControl> m_layerListControl = nullptr;            
            static bool m_isLayerListProcRegistered;

            static LRESULT CALLBACK LayerListProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

        public:
            LayersSection(program::ProgramContext& programContext);
            ~LayersSection();

            void Update() override;
            void HandleSectionResize() override;

            void Refresh(program::LayerManager& layerManager);
            void SetSelectedLayer(size_t layerIndex);
            void RegisterSelectedLayerChangeCallback(std::function<void(size_t)> callback);
            void RegisterLayerVisibilityChangeCallback(std::function<void(size_t, bool)> callback);
    };

}
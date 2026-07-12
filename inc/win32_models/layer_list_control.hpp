#pragma once

#include <windows.h>
#include "program/layer_manager.hpp"
#include "win32_program/win32_context.hpp"

namespace win32_models {

    class LayerListControl
    {
        private:
            HWND m_hwnd;
            std::vector<program::LayerItem> m_layers;            
            size_t m_selectedLayerIndex = 0;

        public:
            LayerListControl(win32_program::MainWindowContext& context, int x, int y, int width, int height);
            ~LayerListControl();

            void Update();
            void Refresh(program::LayerManager& layerManager);

            void SetSelectedLayer(size_t layerIndex);

            size_t GetSelectedLayer() const;        
    };

}
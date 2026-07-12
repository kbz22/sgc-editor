#include "win32_models/layer_list_control.hpp"

win32_models::LayerListControl::LayerListControl(win32_program::MainWindowContext& context, int x, int y, int width, int height)
{
    m_hwnd = CreateWindowExW(
        0,
        L"LayerList",
        L"",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL,
        x, y, width, height,
        context.hMainWindow,
        nullptr,
        context.hInstance,        
        nullptr
    );
}

win32_models::LayerListControl::~LayerListControl()
{
    if (m_hwnd != nullptr)
    {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
}

void win32_models::LayerListControl::Update()
{
    return;
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

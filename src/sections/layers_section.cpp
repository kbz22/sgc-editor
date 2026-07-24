#include "sections/layers_section.hpp"
#include <commctrl.h>
#include "program/program.hpp"

sections::LayersSection::LayersSection(program::ProgramContext& programContext) :
    Section{L"LayerList", win32_program::ControlId::LayerList, *programContext.mainWindowContext}    
{
    RECT rect;
    GetClientRect(GetHwnd(), &rect);

    m_layerListControl = std::make_unique<win32_models::LayerListControl>(
        GetHwnd(),
        programContext.mainWindowContext->hInstance,
        0,
        0,
        rect.right - rect.left,
        rect.bottom - rect.top
    );

    m_layerListControl->SetHImageList(
        programContext.toolbarIcons,
        programContext.toolbarIconsDisabled,
        13,
        14
    );
}

sections::LayersSection::~LayersSection()
{
}

void sections::LayersSection::Update()
{
    Redraw();
    if(m_layerListControl != nullptr) {
        m_layerListControl->Redraw();        
    }    
    return;
}

void sections::LayersSection::Refresh(program::ProgramContext& programContext)
{
    if(m_layerListControl != nullptr) {
        m_layerListControl->Refresh(programContext);
    }
}

void sections::LayersSection::HandleSectionResize()
{
    RECT rect;
    GetClientRect(GetHwnd(), &rect);

    m_layerListControl->Resize(
        rect.right - rect.left,
        rect.bottom - rect.top
    );

    return;
}

void sections::LayersSection::RegisterSelectedLayerChangeCallback(std::function<void(size_t)> callback)
{
    if(m_layerListControl != nullptr) {
        m_layerListControl->RegisterSelectedLayerChangeCallback(callback);
    }
}

void sections::LayersSection::RegisterLayerVisibilityChangeCallback(std::function<void(size_t, bool)> callback)
{
    if(m_layerListControl != nullptr) {
        m_layerListControl->RegisterLayerVisibilityChangeCallback(callback);
    }
}

void sections::LayersSection::SetSelectedLayer(size_t layerIndex)
{
    if(m_layerListControl != nullptr) {
        m_layerListControl->SetSelectedLayer(layerIndex);
    }
}
#include "sections/layers_section.hpp"
#include <commctrl.h>

sections::LayersSection::LayersSection(win32_program::MainWindowContext& context) :
    Section{L"LayerList", win32_program::ControlId::LayerList, context}    
{
    RECT rect;
    GetClientRect(GetHwnd(), &rect);
    m_layerListControl = std::make_unique<win32_models::LayerListControl>(
        GetHwnd(),
        context.hInstance,
        0,
        0,
        rect.right - rect.left,
        rect.bottom - rect.top
    );
}

sections::LayersSection::~LayersSection()
{
}

void sections::LayersSection::Update()
{
    return;
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
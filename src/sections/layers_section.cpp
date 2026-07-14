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
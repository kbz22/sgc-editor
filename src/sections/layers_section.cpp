#include "sections/layers_section.hpp"
#include <commctrl.h>

LRESULT CALLBACK sections::LayersSection::LayerListProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

sections::LayersSection::LayersSection(win32_program::MainWindowContext& context) :
    Section{L"LayerList", win32_program::ControlId::LayerList, context}    
{
    WNDCLASSEX wc = {};
    wc.lpfnWndProc = LayerListProc;
    wc.lpszClassName = L"LayerList";
    wc.hInstance = context.hInstance;
    RegisterClassEx(&wc);

    RECT rect;
    GetClientRect(GetHwnd(), &rect);

    m_layerListControl = std::make_unique<win32_models::LayerListControl>(
        context,
        0, 0,
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

    return;
}
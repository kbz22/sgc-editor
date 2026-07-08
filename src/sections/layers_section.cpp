#include "sections/layers_section.hpp"

sections::LayersSection::LayersSection(win32_program::Win32Context& context) :
    Section{L"LayerList", win32_program::ControlId::LayerList, context}
{}

sections::LayersSection::~LayersSection()
{
}

void sections::LayersSection::Update(program::EditorState state)
{
    return;
}

void sections::LayersSection::HandleSectionResize()
{
    return;
}
#include "sections/package_section.hpp"

sections::PackageSection::PackageSection(win32_program::Win32Context& context) :
    Section{L"PackageList", win32_program::ControlId::PackageView, context}
{}

sections::PackageSection::~PackageSection()
{
}

void sections::PackageSection::Update(program::EditorState state)
{
    return;
}

void sections::PackageSection::HandleSectionResize()
{
    return;
}
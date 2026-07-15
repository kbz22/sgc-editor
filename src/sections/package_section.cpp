#include "sections/package_section.hpp"
#include "program/program.hpp"

sections::PackageSection::PackageSection(program::ProgramContext& programContext) :
    Section{L"PackageList", win32_program::ControlId::PackageView, *programContext.mainWindowContext}
{}

sections::PackageSection::~PackageSection()
{
}

void sections::PackageSection::Update()
{
    Redraw();
    return;
}

void sections::PackageSection::HandleSectionResize()
{
    return;
}
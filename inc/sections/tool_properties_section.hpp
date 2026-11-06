#pragma once

#include "sections/section.hpp"
#include "sections/map_section.hpp"
#include "editor_tools/editor_tool_manager.hpp"
#include <windows.h>

namespace program
{
    class ProgramContext;
}

namespace sections
{
    using editor_tools::EditorToolManager, sections::MapSection, program::ProgramContext;

    class ToolPropertiesSection : public Section
    {
        private:
            EditorToolManager &m_editorToolManager;
            MapSection &m_mapSection;
            HWND m_hwndToolbar;

        public:
            ToolPropertiesSection(ProgramContext &programContext);

            void Update() override;
            void HandleSectionResize() override;
            void Refresh(program::ProgramContext& programContext) override;
    };
}
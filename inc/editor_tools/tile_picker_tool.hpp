#pragma once

#include "editor_tools/ieditor_tool.hpp"
#include "sections/tileset_section.hpp"

namespace editor_tools
{
    class TilePickerTool : public IEditorTool
    {
        private:
            sections::TilesetSection& m_tilesetSection;
            
        public:
            TilePickerTool(sections::TilesetSection& tilesetSection);

            void Execute(file::MapDocument& mapDocument, sgc::tile::TilePosition2D cursorPosition) override;
            void Commit(file::MapDocument& mapDocument) override;
    };
}
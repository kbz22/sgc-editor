#pragma once

#include "editor_tools/ieditor_tool.hpp"
#include "command/paint_command.hpp"
#include "sgc_view/map_view.hpp"
#include "sections/tileset_section.hpp"
#include <memory>
#include <sgc/tile/tile.hpp>

namespace editor_tools
{
    class Brush : public IEditorTool
    {
        protected:
            std::unique_ptr<command::PaintCommand> m_paintCommand;
            std::unique_ptr<sgc::tile::TilePosition2D> m_cursorOrigin;

            sections::TilesetSection &m_tilesetSection;       
            sgc_view::MapView &m_mapView;
            const bool &m_allowChunkCreation;
            bool &m_needsRedraw;

        public:
            Brush(sgc_view::MapView &mapView, sections::TilesetSection &tilesetSection, const bool &allowChunkCreation, bool &needsRedraw);

            virtual void Execute(file::MapDocument& mapDocument, sgc::tile::TilePosition2D cursorPosition) override = 0;
            virtual void Commit(file::MapDocument& mapDocument) override;
    };
}
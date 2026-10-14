#pragma once

#include "editor_tools/ieditor_tool.hpp"
#include "editor_tools/paint_modes.hpp"
#include "sgc_view/map_view.hpp"
#include "program/selection_moved_storage.hpp"
#include "command/paint_selection_command.hpp"
#include "command/bulk_command.hpp"
#include <memory>

namespace sections
{
    class MapSection;
}

namespace editor_tools
{
    class SelectionMoveTool : public IEditorTool
    {
        private:
            sgc_view::MapView &m_mapView;
            sections::MapSection &m_mapSection;
            SelectionMode &m_selectionMode;
            sgc::tile::TileId &m_clearTileId;
            bool &m_allowChunkCreation;
            std::unique_ptr<sgc::tile::TilePosition2D> m_movingSelectionOffset;            
            std::unique_ptr<program::SelectionMovedStorage> m_selectionMovedStorage;
            std::unique_ptr<command::PaintSelectionCommand> m_clearSelectionCommand;
            std::unique_ptr<command::PaintSelectionCommand> m_placeSelectionCommand;
            std::unique_ptr<command::BulkCommand> m_bulkCommand;

        public:
            SelectionMoveTool(sgc_view::MapView &mapView, sections::MapSection &mapSection, SelectionMode &selectionMode, sgc::tile::TileId &clearTileId, bool &allowChunkCreation);

            void Execute(file::MapDocument& mapDocument, sgc::tile::TilePosition2D cursorPosition);
            void Commit(file::MapDocument& mapDocument);
    };
}
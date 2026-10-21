#pragma once

#include "file/map_document.hpp"
#include "file/asset_manager.hpp"
#include "sgc_view/map_view.hpp"
#include "sgc_view/tileset_view.hpp"
#include "sections/tileset_section.hpp"
#include "command/paint_command.hpp"
#include "editor_tools/ieditor_tool.hpp"
#include "editor_tools/editor_tools.hpp"
#include "editor_tools/paint_modes.hpp"
#include <sgc/graphics/rectangle.hpp>
#include <sgc/graphics/tileset.hpp>
#include <sgc/tile/tile.hpp>
#include <memory>
#include <vector>
#include <chrono>
#include <unordered_map>

namespace sections {
    class MapSection;
}

namespace editor_tools {

    class EditorToolManager
    {
        private:
            PaintMode m_paintMode{PaintMode::Brush};
            SelectionMode m_selectionMode{SelectionMode::SingleLayer};
            bool m_allowChunkCreation = false;
            sgc::tile::TileId m_clearTile = 0;
            bool m_needsRedraw = false;
            std::unique_ptr<sgc::tile::TilePosition2D> m_lastPosition{nullptr};
            sections::TilesetSection &m_tilesetSection;

            IEditorTool *m_activeTool{nullptr};
            PaintBrush m_paintBrush;
            RectangularBrush m_rectangularBrush;
            FillTool m_fillTool;
            ChunkRemoverTool m_chunkRemover;
            TilePickerTool m_tilePickerTool;
            SelectionTool m_selectionTool;
            SelectionMoveTool m_selectionMoveTool;
            ScatterBrush m_scatterBrush;

            std::unordered_map<IEditorTool*, std::chrono::milliseconds> m_toolTimeIntervals;
            std::chrono::steady_clock::time_point m_lastCheckTimestamp;

            void SetActiveTool();

        public:
            EditorToolManager(sgc_view::MapView &mapView, sections::MapSection &mapSection, sections::TilesetSection &tilesetSection);
            ~EditorToolManager() = default;

            PaintMode GetPaintMode() const;
            SelectionMode GetSelectionMode() const;
            bool NeedsRedraw() const;
            bool NeedsUpdatesPeriodically() const;

            void SetPaintMode(PaintMode paintMode);
            void SetModeDelay(PaintMode paintMode, std::chrono::milliseconds interval);
            void SetSelectionMode(SelectionMode selectionMode);
            void SetAllowChunkCreation(bool check);

            void Execute(file::MapDocument& mapDocument, sgc::tile::TilePosition2D cursorPosition);
            void Commit(file::MapDocument& mapDocument);
    };

}
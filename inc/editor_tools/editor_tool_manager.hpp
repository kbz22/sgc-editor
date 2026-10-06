#pragma once

#include "file/map_document.hpp"
#include "file/asset_manager.hpp"
#include "sgc_view/map_view.hpp"
#include "sgc_view/tileset_view.hpp"
#include "sections/tileset_section.hpp"
#include "command/paint_command.hpp"
#include "editor_tools/ieditor_tool.hpp"
#include "editor_tools/editor_tools.hpp"
#include <sgc/graphics/rectangle.hpp>
#include <sgc/graphics/tileset.hpp>
#include <sgc/tile/tile.hpp>
#include <memory>
#include <vector>

namespace sections {
    class MapSection;    
}

namespace editor_tools {

    enum class PaintMode
    {
        Brush,
        Rectangle,
        Fill,
        Select,
        ChunkRemover,
        TilePicker,
    };

    enum class SelectionMode
    {
        SingleLayer,
        AllLayers,
        VisibleLayers
    };

    class EditorToolManager
    {
        private:
            PaintMode m_paintMode{PaintMode::Brush};            
            SelectionMode m_selectionMode{SelectionMode::SingleLayer};            
            bool m_allowChunkCreation = false;
            sgc::tile::TileId m_clearTile = 0;
            bool m_needsRedraw = false;
            std::unique_ptr<sgc::tile::TilePosition2D> m_lastPosition{nullptr};

            IEditorTool *m_activeTool{nullptr};
            PaintBrush m_paintBrush;
            RectangularBrush m_rectangularBrush;
            FillTool m_fillTool;
            ChunkRemoverTool m_chunkRemover;

            void SetActiveTool();

        public:
            EditorToolManager(sgc_view::MapView &mapView, sections::TilesetSection &tilesetSection);
            ~EditorToolManager() = default;

            PaintMode GetPaintMode() const;
            SelectionMode GetSelectionMode() const;
            bool NeedsRedraw() const;

            void SetPaintMode(PaintMode paintMode);            
            void SetSelectionMode(SelectionMode selectionMode);
            void SetCheckTileBeforePainting(bool check);          

            void Execute(file::MapDocument& mapDocument, sgc::tile::TilePosition2D cursorPosition);
            void Commit(file::MapDocument& mapDocument);
    };

}
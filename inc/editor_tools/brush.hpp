#pragma once

#include "file/map_document.hpp"
#include "command/paint_command.hpp"
#include <sgc/graphics/rectangle.hpp>
#include <sgc/graphics/tileset.hpp>
#include <sgc/tile/tile.hpp>
#include <memory>
#include <vector>

namespace editor_tools {

    enum class PaintMode
    {
        Brush,
        Rectangle,
        Fill,
        Select,
        Eraser
    };

    class Brush
    {
        private:
            PaintMode m_paintMode{PaintMode::Brush};
            sgc::graphics::Rectangle& m_selectionRectangleOnTileset;            
            std::unique_ptr<sgc::tile::TilePosition2D> m_selectionStart{nullptr};
            std::unique_ptr<sgc::tile::TilePosition2D> m_lastSelection{nullptr};
            std::unique_ptr<command::PaintCommand> m_paintCommand{nullptr};
            bool m_checkTileBeforePainting{true};
            bool m_needsRedraw{false};

        public:
            Brush(sgc::graphics::Rectangle& selectionRectangleOnTileset);
            ~Brush() = default;

            PaintMode GetPaintMode() const;
            bool NeedsRedraw() const;

            void SetPaintMode(PaintMode paintMode);
            void SetCheckTileBeforePainting(bool check);

            void PaintExecuteChange(
                file::MapDocument& mapDocument,
                sgc::graphics::Tileset& tileset,
                sgc::tile::TilePosition2D tilePosition,
                sgc::tile::TilePosition2D cursorPositionOnTileset,
                sgc::tile::TileSize2D tileSize
            );
            void PaintCommitChanges(
                file::MapDocument& mapDocument
            );

            friend void PaintStroke(
                Brush& brush,
                file::MapDocument& mapDocument,
                sgc::graphics::Tileset& tileset,
                sgc::tile::TilePosition2D tilePosition,
                sgc::tile::TilePosition2D cursorPositionOnTileset,
                sgc::tile::TileSize2D tileSize
            );
            friend void PaintRectangle(
                Brush& brush,
                file::MapDocument& mapDocument,
                sgc::graphics::Tileset& tileset,
                sgc::tile::TilePosition2D tilePosition,
                sgc::tile::TilePosition2D cursorPositionOnTileset,
                sgc::tile::TileSize2D tileSize
            );
            friend void PaintFill(
                Brush& brush,
                file::MapDocument& mapDocument,
                sgc::graphics::Tileset& tileset,
                sgc::tile::TilePosition2D tilePosition,
                sgc::tile::TilePosition2D cursorPositionOnTileset,
                sgc::tile::TileSize2D tileSize
            );
    };

}
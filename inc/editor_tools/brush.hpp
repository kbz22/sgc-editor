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
        Select
    };

    enum class EraserMode
    {
        None,
        ClearTile,
        DeleteChunk
    };

    class Brush
    {
        private:
            PaintMode m_paintMode{PaintMode::Brush};
            EraserMode m_eraserMode{EraserMode::None};
            sgc::graphics::Rectangle& m_selectionRectangleOnTileset;            
            std::unique_ptr<sgc::tile::TilePosition2D> m_selectionStart{nullptr};
            std::unique_ptr<sgc::tile::TilePosition2D> m_lastSelection{nullptr};
            std::unique_ptr<command::PaintCommand> m_paintCommand{nullptr};
            bool m_checkTileBeforePainting{true};
            bool m_needsRedraw{false};            
            sgc::tile::TileId m_clearTileId{0};

        public:
            Brush(sgc::graphics::Rectangle& selectionRectangleOnTileset);
            ~Brush() = default;

            PaintMode GetPaintMode() const;
            EraserMode GetEraserMode() const;
            bool NeedsRedraw() const;

            void SetPaintMode(PaintMode paintMode);
            void SetEraserMode(EraserMode eraserMode);
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
            friend void EraseStroke(
                Brush& brush,
                file::MapDocument& mapDocument,
                sgc::graphics::Tileset& tileset,
                sgc::tile::TilePosition2D tilePosition,
                sgc::tile::TilePosition2D cursorPositionOnTileset,
                sgc::tile::TileSize2D tileSize
            );
            friend void EraseRectangle(
                Brush& brush,
                file::MapDocument& mapDocument,
                sgc::graphics::Tileset& tileset,
                sgc::tile::TilePosition2D tilePosition,
                sgc::tile::TilePosition2D cursorPositionOnTileset,
                sgc::tile::TileSize2D tileSize
            );
            friend void EraseFill(
                Brush& brush,
                file::MapDocument& mapDocument,
                sgc::graphics::Tileset& tileset,
                sgc::tile::TilePosition2D tilePosition,
                sgc::tile::TilePosition2D cursorPositionOnTileset,
                sgc::tile::TileSize2D tileSize
            );
    };

}
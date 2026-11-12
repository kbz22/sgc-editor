#pragma once

#include <memory>
#include <windows.h>
#include <sgc/math/vector.hpp>
#include <sgc/tile/tile.hpp>
#include <optional>

#include "sections/section.hpp"
#include "sgc_view/tileset_view.hpp"
#include "program/tile_symbol_manager.hpp"

namespace program {
    class ProgramContext;
}

namespace sections {

    class TilesetSection : public Section
    {
        private:
            std::unique_ptr<sgc_view::TilesetView> m_tilesetView = nullptr;

            LRESULT HandleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
            bool m_selectionActive = false;
            bool m_symbolDrawingEnabled = false;            
            program::TileSymbolManager* m_tileSymbolManager;

            sgc::tile::TilePosition2D m_selectionTileStart = { 0, 0 };
            sgc::tile::TilePosition2D m_selectionTileSize = { 0, 0 };

            void UpdateStatusBar(sgc::math::vec2 position, std::optional<sgc::tile::TileId> tileId, sgc::math::vec2 size);
        
        public:
            TilesetSection(program::ProgramContext& programContext);
            
            void Update() override;
            void HandleSectionResize() override;
            void Refresh(program::ProgramContext& programContext) override;

            void ResetCursorSize();
            void SetCursorTileId(sgc::tile::TileId tileId);
            void EnableSymbolDrawing(bool enable = true);            
            void NextSymbolGroup();
            void NextSymbol();

            bool IsReady() const;
            bool IsDrawingWithSymbols() const;            

            sgc::graphics::PixelPosition2D GetCursorPositionInPixels() const;
            sgc::graphics::PixelSize2D GetCursorSizeInPixels() const;
            sgc::tile::TilePosition2D GetCursorPositionInTiles() const;
            sgc::tile::TileSize2D GetCursorSizeInTiles() const;
            sgc::tile::TileId GetClearTileId() const;

            program::SgcTileset& GetSgcTileset();

            void ClearTileset();
    };

}
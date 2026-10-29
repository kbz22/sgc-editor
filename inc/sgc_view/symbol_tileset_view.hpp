#pragma once

#include "sgc_view/sgc_view.hpp"
#include <sgc/tile/tile.hpp>
#include <sgc/graphics/rectangle.hpp>
#include <vector>

namespace sgc_view
{
    class SymbolTilesetView : public SgcView
    {
        private:
            std::shared_ptr<graphics::TiledLayer> m_layer = nullptr;
            std::unique_ptr<graphics::Rectangle> m_symbolIndicator = nullptr;
            std::unique_ptr<graphics::Rectangle> m_cursor = nullptr;            
            std::vector<sgc::graphics::PixelPosition2D> m_symbolPositions{};
            sgc::graphics::color m_cursorColor{ 251, 192, 27, 255 };
            bool m_cursorVisible = false;

            void ResetTileset();

        public:
            SymbolTilesetView(HWND hwnd);
            ~SymbolTilesetView() = default;

            void Render() override;
            void Refresh(program::ProgramContext& programContext) override;
            void SetTileset(sgc::data::AssetId tilesetId) override;

            void AddTilePosition(sgc::tile::TilePosition2D position);
            void AddTilePosition(std::vector<sgc::tile::TilePosition2D> &positions);
            void SetSizeInTiles(sgc::tile::TileSize2D size);
            void SetCursorColor(sgc::graphics::color color);
            void SetIndicatorColor(sgc::graphics::color color);            
            void ShowCursor(bool value);
            void SetCursorPosition(sgc::tile::TilePosition2D position);

            std::vector<sgc::tile::TilePosition2D> ClearSymbolPositions();
            std::vector<sgc::tile::TilePosition2D> GetSymbolPositions() const;
            sgc::tile::TilePosition2D GetCursorPosition() const;
            sgc::tile::TilePosition2D GetSymbolSize() const;
            sgc::graphics::PixelPosition2D GetImageSize() const;
            sgc::graphics::PixelPosition2D GetTilesetImageSize() const;

            bool Empty();
            bool IsCursorVisible();
    };
}
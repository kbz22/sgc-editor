#pragma once

#include <sgc_view/sgc_view.hpp>
#include <sgc/graphics/rectangle.hpp>
#include <sgc/graphics/tiledlayer.hpp>
#include "defaults.hpp"

namespace sgc_view 
{
    class TilesetView : public SgcView
    {
        private:
            math::uval m_gridWidth = 0;
            math::uval m_gridHeight = 0;
            std::shared_ptr<graphics::TiledLayer> m_layer = nullptr;
            std::unique_ptr<graphics::Rectangle> m_cursorTile;
            sgc::graphics::color m_cursorColor{ 0, 128, 255, 128 };

            void SetTileset(sgc::data::AssetId tilesetId) override;
            void ResetTileset();

            void SetCursorTile(sgc::graphics::PixelPosition2D position, sgc::graphics::PixelSize2D size);
            void ResetCursorTile();

        public:
            TilesetView(HWND hwnd);
            ~TilesetView();

            void Render() override;
            void Refresh(program::ProgramContext& programContext) override;

            void SetCursorPositionInPixels(sgc::graphics::PixelPosition2D position);
            void SetCursorSizeInPixels(sgc::graphics::PixelSize2D size);

            sgc::graphics::PixelPosition2D GetCursorPositionInPixels() const;
            sgc::graphics::PixelSize2D GetCursorSizeInPixels() const;

            void SetCursorColor(sgc::graphics::color color);
            
    };
}
#pragma once

#include "sgc_view/sgc_view.hpp"
#include "program/layer_manager.hpp"

#include <sgc/math/vector.hpp>
#include <sgc/data/chunkedtilestorage.hpp>
#include <sgc/graphics/rectangle.hpp>

namespace sections {
    class MapSection;
}

namespace sgc_view
{
    using namespace sgc;

    class MapView : public SgcView
    {
        private:            
            graphics::Rectangle m_cursorTile;
            void SetTileset(sgc::data::AssetId tilesetId) override;

        public:
            MapView(HWND hwnd);
            ~MapView();
            
            void Render() override;
            void Refresh(program::ProgramContext& programContext) override;

            void SetCursorPositionInPixels(sgc::graphics::PixelPosition2D position);
            void SetCursorSizeInPixels(sgc::graphics::PixelSize2D size);
            void SetCameraPositionSingles(float x, float y);

            void ChangeCameraPositionSingles(float deltaX, float deltaY);
            void ChangeCursorPositionInPixels(sgc::graphics::PixelPosition2D delta);

            sgc::graphics::PixelPosition2D GetCursorPositionInPixels() const;            
            sgc::tile::TilePosition2D GetCursorPositionInTiles() const;
            sgc::graphics::PixelSize2D GetCursorSizeInPixels() const;
            sgc::tile::TileSize2D GetCursorSizeInTiles() const;
            sgc::math::fvec2 GetCameraPositionSingles() const;
            sgc::graphics::View GetView() const;

            friend class sections::MapSection;
    };
}
#pragma once

#include "sgc_view/sgc_view.hpp"
#include "program/layer_manager.hpp"

#include <sgc/math/vector.hpp>
#include <sgc/data/chunkedtilestorage.hpp>
#include <sgc/graphics/rectangle.hpp>
#include <sgc/graphics/linegrid.hpp>
#include <functional>

namespace sections {
    class MapSection;
}

namespace sgc_view
{
    using namespace sgc;

    class MapView : public SgcView
    {
        private:
            std::unique_ptr<graphics::Rectangle> m_cursorTile;
            std::shared_ptr<graphics::LineGrid> m_tileGrid;
            std::shared_ptr<graphics::LineGrid> m_chunkGrid;
            std::function<void(sgc::math::vec2)> m_onCursorPositionChangedCallback = nullptr;
            float m_zoom = 1.0f;

            void SetTileset(sgc::data::AssetId tilesetId) override;
            void ResetTileset();

            void SetCursorTile(sgc::graphics::PixelSize2D size);
            void ResetCursorTile();

        public:
            MapView(HWND hwnd);
            ~MapView();
            
            void Render() override;
            void Refresh(program::ProgramContext& programContext) override;
            void SetScreenSize(int width, int height) override;

            void UpdateGridPosition(sgc::graphics::Viewport cameraViewport);

            bool SetZoom(float zoom); // returns true if zoom was changed
            void SetCursorPositionInPixels(sgc::graphics::PixelPosition2D position);
            void SetCursorSizeInPixels(sgc::graphics::PixelSize2D size);
            void SetCameraPositionSingles(float x, float y);

            void ChangeCameraPositionSingles(float deltaX, float deltaY);
            void ChangeCursorPositionInPixels(sgc::graphics::PixelPosition2D delta);

            void RegisterOnCursorPositionChangedCallback(std::function<void(sgc::math::vec2)> callback);

            float GetZoom() const;
            sgc::graphics::PixelPosition2D GetCursorPositionInPixels() const;
            sgc::tile::TilePosition2D GetCursorPositionInTiles() const;
            sgc::graphics::PixelSize2D GetCursorSizeInPixels() const;
            sgc::tile::TileSize2D GetCursorSizeInTiles() const;
            sgc::math::fvec2 GetCameraPositionSingles() const;
            sgc::math::fvec2 GetCursorPositionSingles() const;
            sgc::graphics::View GetView() const;

            inline float ScaleForZoom(float value) const
            {
                return value / m_zoom;
            }

            friend class sections::MapSection;
    };
}
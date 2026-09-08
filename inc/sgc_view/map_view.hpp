#pragma once

#include "sgc_view/sgc_view.hpp"
#include "program/layer_manager.hpp"
#include "sgc_extension/marching_ants_rectangle.hpp"

#include <sgc/math/vector.hpp>
#include <sgc/data/chunkedtilestorage.hpp>
#include <sgc/graphics/rectangle.hpp>
#include <sgc/graphics/linegrid.hpp>
#include <sgc/graphics/drawablelayers.hpp>
#include <functional>

namespace sections {
    class MapSection;
}

namespace sgc_view
{
    using namespace sgc;

    enum class CursorTileMode {
        Invisible,
        Default,
        RemoveTile,
        RemoveChunk
    };

    class MapView : public SgcView
    {
        private:
            std::unique_ptr<graphics::Rectangle> m_cursorTile;
            CursorTileMode m_cursorTileMode = CursorTileMode::Default;
            std::shared_ptr<graphics::MarchingAntsRectangle> m_marchingAntsRectangleOnMap;
            std::shared_ptr<graphics::DrawableLayers> m_mapLayers;
            std::shared_ptr<graphics::DrawableLayers> m_selectionLayers;
            std::shared_ptr<graphics::LineGrid> m_tileGrid;
            std::shared_ptr<graphics::LineGrid> m_chunkGrid;
            std::function<void(sgc::math::vec2)> m_onCursorPositionChangedCallback = nullptr;
            std::function<void(sgc::math::vec2)> m_onSelectionSizeChangedCallback = nullptr;
            float m_zoom = 1.0f;
            std::chrono::steady_clock::time_point m_lastSelectionOffsetUpdateTime;
            float m_offset = 0.0f;

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

            void RenderToImage(std::filesystem::path outputPath);

            void UpdateGridPosition(sgc::graphics::Viewport cameraViewport);
            void UpdateSelectionOffset();

            bool SetZoom(float zoom); // returns true if zoom was changed
            void SetCursorPositionInPixels(sgc::graphics::PixelPosition2D position);
            void SetCursorSizeInPixels(sgc::graphics::PixelSize2D size);
            void SetSelectionPositionInPixels(sgc::graphics::PixelPosition2D position);
            void SetSelectionSizeInPixels(sgc::graphics::PixelSize2D size);
            void SetCameraPositionSingles(float x, float y);
            void SetCursorMode(CursorTileMode mode);            

            void ChangeCameraPositionSingles(float deltaX, float deltaY);
            void ChangeCursorPositionInPixels(sgc::graphics::PixelPosition2D delta);

            void RegisterOnCursorPositionChangedCallback(std::function<void(sgc::math::vec2)> callback);
            void RegisterOnSelectionSizeChangedCallback(std::function<void(sgc::math::vec2)> callback);

            float GetZoom() const;
            sgc::graphics::PixelPosition2D GetCursorPositionInPixels() const;
            sgc::tile::TilePosition2D GetCursorPositionInTiles() const;
            sgc::graphics::PixelSize2D GetCursorSizeInPixels() const;
            sgc::tile::TileSize2D GetCursorSizeInTiles() const;
            sgc::math::fvec2 GetCameraPositionSingles() const;
            sgc::math::fvec2 GetCursorPositionSingles() const;
            sgc::graphics::View GetView() const;
            sgc::tile::TilePosition2D GetSelectionPositionInTiles() const;
            sgc::tile::TileSize2D GetSelectionSizeInTiles() const;
            graphics::DrawableLayers* GetSelectionLayers() const;

            inline float ScaleForZoom(float value) const
            {
                return value / m_zoom;
            }

            friend class sections::MapSection;
    };
}
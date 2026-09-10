#pragma once

#include "sections/section.hpp"
#include "sgc_view/map_view.hpp"
#include "command/paint_command.hpp"
#include "editor_tools/brush.hpp"
#include <sgc/data/statictilestorage.hpp>
#include <functional>
#include <mutex>
#include <windows.h>

namespace program {
    class ProgramContext;
}

namespace sections {

    class MapSection : public Section
    {
        private:
            std::unique_ptr<sgc_view::MapView> m_mapView = nullptr;            
            bool m_isPainting = false;
            bool m_isPanning = false;
            bool m_isCaptured = false;
            bool m_isMovingSelection = false;
            bool m_canMoveSelection = false;
            bool m_needsRedraw = false;
            std::mutex m_needsRedrawMutex{};
            sgc::math::vec2 m_lastMousePosPan = { 0, 0 };
            sgc::math::vec2 m_movingSelectionOffset = { 0, 0 };
            std::function<void(float)> m_onZoomChangedCallback = nullptr;
            std::map<size_t, std::shared_ptr<sgc::data::StaticTileStorage>> m_selectionMovedStorage;
            std::chrono::milliseconds m_timeBetweenUpdates = std::chrono::milliseconds(8);
            std::chrono::milliseconds m_selectionRectUpdateInterval = std::chrono::milliseconds(50);
            std::chrono::steady_clock::time_point m_lastUpdateTime = std::chrono::steady_clock::now();

            editor_tools::Brush m_brush;

        protected:
            LRESULT HandleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;

        public:
            MapSection(program::ProgramContext& programContext);
            
            void Update() override;
            void HandleSectionResize() override;            

            void Refresh(program::ProgramContext& programContext) override;

            void RenderToImage(std::filesystem::path outputPath);

            void ExecuteZoom(sgc::math::fvec2 anchorPoint, float zoomValue);

            void ResetSelection();            
            
            void SetCheckTileBeforePainting(bool check);
            void SetPaintMode(editor_tools::PaintMode paintMode);
            void SetEraseMode(editor_tools::EraserMode eraserMode);
            void SetSelectionMode(editor_tools::SelectionMode selectionMode);
            void SetSelectionMoveMode(bool canMoveSelection);

            void SetSelectionPositionInTiles(sgc::tile::TilePosition2D position);
            void SetSelectionSizeInTiles(sgc::tile::TileSize2D size);

            void RegisterOnZoomChangedCallback(std::function<void(float)> callback);
            
            editor_tools::PaintMode GetPaintMode() const;
            editor_tools::EraserMode GetEraseMode() const;
            editor_tools::SelectionMode GetSelectionMode() const;            
            float GetZoom() const;
            sgc::math::fvec2 GetScreenCenterWorldPosition() const;
            bool GetSelectionMoveMode() const;
            
            sgc::tile::TilePosition2D GetSelectionRectanglePositionTiles() const;
            sgc::tile::TileSize2D GetSelectionRectangleSizeTiles() const;

            sgc::tile::TilePosition2D GetCursorPositionInTiles() const;

            bool IsSelectionActive() const;
    };

}
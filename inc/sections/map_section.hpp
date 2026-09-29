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

    enum class PointerType {        
        LeftMouse,
        MiddleMouse,
        RightMouse,
        Touch,
        Pen
    };

    inline bool IsMouse(PointerType pointerType) {
        return pointerType == PointerType::LeftMouse ||
               pointerType == PointerType::MiddleMouse ||
               pointerType == PointerType::RightMouse;
    }

    class PointerLock 
    {
        private:
            PointerType m_pointerType = PointerType::LeftMouse;
            bool m_isLocked = false;

        public:
            bool IsLocked() const;
            bool IsLockedBy(PointerType pointerType) const;

            bool Acquire(PointerType pointerType);
            bool Release(PointerType pointerType);
    };

    class MapSection : public Section
    {
        private:
            std::unique_ptr<sgc_view::MapView> m_mapView = nullptr;
            PointerLock m_isPainting;
            PointerLock m_isMovingSelection;
            PointerLock m_isPanning;
            PointerLock m_isCaptured;
            PointerType m_paintingPointerType = PointerType::LeftMouse;
            PointerType m_panningPointerType = PointerType::MiddleMouse;
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

            bool UpdateCursorPosition(sgc::graphics::PixelPosition2D pointerPosition, program::ProgramContext& programContext);            
            bool UpdateSelectionMove(PointerType pointerType, sgc::graphics::PixelPosition2D pointerPosition);
            bool UpdateDragDrawing(PointerType pointerType, program::ProgramContext& programContext);
            bool UpdateOnCursorDown(PointerType pointerType, file::MapDocument *mapDocument, program::ProgramContext& programContext);
            bool UpdateOnCursorUp(PointerType pointerType, file::MapDocument *mapDocument);

            void PanningDown(PointerType pointerType, sgc::graphics::PixelPosition2D position);
            void PanningUp(PointerType pointerType);
            bool PanningUpdate(PointerType pointerType, sgc::graphics::PixelPosition2D position);

            void PointerDown(PointerType pointerType, file::MapDocument *mapDocument, sgc::graphics::PixelPosition2D position, program::ProgramContext& programContext);
            void PointerUp(PointerType pointerType, file::MapDocument *mapDocument, program::ProgramContext& programContext);            
            void PointerUpdate(PointerType pointerType, sgc::graphics::PixelPosition2D pointerPosition, program::ProgramContext& programContext);

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

            void SetPaintingPointerType(PointerType pointerType);
            void SetPanningPointerType(PointerType pointerType);

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

            PointerType GetPaintingPointerType() const;
            PointerType GetPanningPointerType() const;
            
            bool IsSelectionActive() const;
    };

}
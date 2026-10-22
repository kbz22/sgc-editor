#pragma once

#include "sections/section.hpp"
#include "sgc_view/map_view.hpp"
#include "command/paint_command.hpp"
#include "editor_tools/editor_tool_manager.hpp"
#include "program/selection_moved_storage.hpp"
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
        Pen,

        Count
    };

    enum class PointerBehaviour
    {
        None = -1,
        Panning,
        Painting
    };

    struct Pointer
    {
        PointerType Type = PointerType::LeftMouse;
        uint32_t Id = 0;

        bool operator==(const Pointer& other) const = default;
    };

    inline bool IsMouse(const PointerType &pointerType) {
        return pointerType == PointerType::LeftMouse ||
               pointerType == PointerType::MiddleMouse ||
               pointerType == PointerType::RightMouse;
    }

    inline bool IsMouse(const Pointer &pointer) {
        return IsMouse(pointer.Type);
    }

    class PointerLock 
    {
        private:
            // PointerType m_pointerType = PointerType::LeftMouse;
            Pointer m_pointer;
            bool m_isLocked = false;

        public:
            bool IsLocked() const;
            bool IsLockedBy(Pointer pointer) const;

            PointerType GetPointerType() const;
            uint32_t GetPointerId() const;

            bool Acquire(Pointer pointer);
            bool Release(Pointer pointer);
    };    

    class MapSection : public Section
    {
        private:
            std::unique_ptr<sgc_view::MapView> m_mapView = nullptr;
            PointerLock m_isPainting;
            PointerLock m_isMovingSelection;
            PointerLock m_isPanning;
            PointerLock m_isCaptured;
            std::unordered_map<PointerType, PointerBehaviour> m_pointerBehaviour;
            bool m_canMoveSelection = false;
            bool m_needsRedraw = false;
            std::mutex m_needsRedrawMutex{};
            sgc::math::vec2 m_lastMousePosPan = { 0, 0 };
            sgc::math::vec2 m_movingSelectionOffset = { 0, 0 };
            std::function<void(float)> m_onZoomChangedCallback = nullptr;
            // std::map<size_t, std::shared_ptr<sgc::data::StaticTileStorage>> m_program::SelectionMovedStorage;
            std::chrono::milliseconds m_timeBetweenUpdates = std::chrono::milliseconds(8);
            std::chrono::milliseconds m_selectionRectUpdateInterval = std::chrono::milliseconds(50);
            std::chrono::milliseconds m_editorToolsUpdateInterval = std::chrono::milliseconds(50);
            std::chrono::steady_clock::time_point m_lastUpdateTime = std::chrono::steady_clock::now();
            
            editor_tools::EditorToolManager m_editorToolManager;

            bool UpdateCursorPosition(sgc::graphics::PixelPosition2D pointerPosition, program::ProgramContext& programContext);
            bool UpdateSelectionMove(Pointer pointer, file::MapDocument *mapDocument, sgc::graphics::PixelPosition2D pointerPosition);
            bool UpdateDragDrawing(Pointer pointer, program::ProgramContext& programContext);
            bool UpdateOnCursorDown(Pointer pointer);
            bool UpdateOnCursorUp(Pointer pointer, file::MapDocument *mapDocument, program::ProgramContext& programContext);

            void PanningDown(Pointer pointer, sgc::graphics::PixelPosition2D position);
            void PanningUp(Pointer pointer);
            bool PanningUpdate(Pointer pointer, sgc::graphics::PixelPosition2D position);

            void PointerDown(Pointer pointer, sgc::graphics::PixelPosition2D position, program::ProgramContext& programContext);
            void PointerUp(Pointer pointer, file::MapDocument *mapDocument, program::ProgramContext& programContext);
            void PointerUpdate(Pointer pointer, sgc::graphics::PixelPosition2D pointerPosition, program::ProgramContext& programContext);

            inline bool IsPointerPanning(PointerType pointerType);
            inline bool IsPointerPainting(PointerType pointerType);

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
            
            void SetAllowChunkCreation(bool check);
            void SetPaintMode(editor_tools::PaintMode paintMode);
            void SetSelectionMode(editor_tools::SelectionMode selectionMode);
            void SetSelectionMoveMode(bool canMoveSelection);

            void SetSelectionPositionInTiles(sgc::tile::TilePosition2D position);
            void SetSelectionSizeInTiles(sgc::tile::TileSize2D size);

            void SetPointerBehaviour(PointerType pointerType, PointerBehaviour behaviour);

            void SetSelectionMovedImage(program::SelectionMovedStorage &storageLayers);
            void UpdateSelectionMovedPosition(sgc::tile::TilePosition2D positionOnMap);

            void RegisterOnZoomChangedCallback(std::function<void(float)> callback);
            
            editor_tools::PaintMode GetPaintMode() const;
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
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

    enum class PointerType : uint8_t {
        Mouse,
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
        PointerType Type = PointerType::Mouse;
        uint8_t Button = 0;
        uint32_t Id = 0;

        bool operator==(const Pointer& other) const = default;
    };

    struct PointerMapKey
    {
        PointerType Type;
        uint8_t Button;

        bool operator==(const PointerMapKey& other) const = default;
        bool operator==(const Pointer& other) const
        {
            return this->Type == other.Type && this->Button == other.Button;
        };
        PointerMapKey& operator=(const Pointer& other)
        {
            Type = other.Type;
            Button = other.Button;
            return *this;
        }
    };

}

namespace std {
    template <>
    struct hash<sections::PointerMapKey>
    {
        std::size_t operator()(const sections::PointerMapKey& k) const
        {
            return (static_cast<std::size_t>(k.Type) << 32) ^ static_cast<std::size_t>(k.Button);
        }
    };
}

namespace sections {

    inline std::vector<Pointer> BuildPointersDown(PointerType type, uint32_t id, uint32_t flags)
    {
        std::vector<Pointer> pointers;

        if(flags & POINTER_FLAG_FIRSTBUTTON)
            pointers.push_back({ type, 0, id });

        if(flags & POINTER_FLAG_SECONDBUTTON)
            pointers.push_back({ type, 1, id });

        if(flags & POINTER_FLAG_THIRDBUTTON)
            pointers.push_back({ type, 2, id });

        if(flags & POINTER_FLAG_FOURTHBUTTON)
            pointers.push_back({ type, 3, id });

        if(flags & POINTER_FLAG_FIFTHBUTTON)
            pointers.push_back({ type, 4, id });

        return pointers;
    }

    inline std::vector<Pointer> BuildPointersUp(PointerType type, uint32_t id, uint32_t flags)
    {
        std::vector<Pointer> pointers;

        if(!(flags & POINTER_FLAG_FIRSTBUTTON))
            pointers.push_back({ type, 0, id });

        if(!(flags & POINTER_FLAG_SECONDBUTTON))
            pointers.push_back({ type, 1, id });

        if(!(flags & POINTER_FLAG_THIRDBUTTON))
            pointers.push_back({ type, 2, id });

        if(!(flags & POINTER_FLAG_FOURTHBUTTON))
            pointers.push_back({ type, 3, id });

        if(!(flags & POINTER_FLAG_FIFTHBUTTON))
            pointers.push_back({ type, 4, id });

        return pointers;
    }

    inline std::vector<Pointer> BuildAllPointers(PointerType type, uint32_t id)
    {
        std::vector<Pointer> pointers;

        for(uint8_t i=0; i<5; i++)
            pointers.push_back({ type, i, id });

        return pointers;
    }

    class PointerLock 
    {
        private:
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
            std::unordered_map<PointerMapKey, PointerBehaviour> m_pointerBehaviour;
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

            inline bool IsPointerPanning(Pointer pointer);
            inline bool IsPointerPainting(Pointer pointer);

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

            void SetPointerBehaviour(PointerMapKey pointerKey, PointerBehaviour behaviour);

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
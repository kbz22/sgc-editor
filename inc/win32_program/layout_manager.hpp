#pragma once

#include "defaults.hpp"
#include "program/program.hpp"
#include <windows.h>

namespace win32_program {

    enum class DraggedSplitter
    {
        None,
        LayerPackage,
        TilesetMap,
        LayerMap
    };

    class LayoutManager
    {
        private:
            HWND m_layerPackageSplitter = HWND();
            HWND m_tilesetMapSplitter = HWND();
            HWND m_layerMapSplitter = HWND();

            float m_layersPackageViewRatio = defaults::initialLayerHorizontalRatio;
            float m_tilesetMapRatio = defaults::initialTilesetRatio;
            float m_layersMapViewRatio = defaults::initialLayerVerticalRatio;            

            DraggedSplitter m_draggedSplitter = DraggedSplitter::None;

            int m_toolbarOffset = 0;
            int m_splitH = 5;
            int m_splitW = 5;

            program::ProgramContext& m_programContext;            

        public:
            LayoutManager(program::ProgramContext& programContext = program::GetProgramContext());              

            void CheckDragging(HWND hwnd, LPARAM lParam);
            void HandleResize(HWND hwnd, LPARAM lParam);
            void HandleDragging(HWND hwnd, LPARAM lParam);
            
    };

}
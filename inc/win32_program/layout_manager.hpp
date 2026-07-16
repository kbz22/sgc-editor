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
            int m_windowWidth = 0;
            int m_windowHeight = 0;
            int m_menuBarHeight = 0;
            int m_toolbarHeight = 0;

            program::ProgramContext& m_programContext; 
            
            void UpdateRebarLayout(HWND hwnd, int width);

        public:
            LayoutManager(program::ProgramContext& programContext = program::GetProgramContext());              

            DraggedSplitter GetDraggedSplitter(HWND hwnd, LPARAM lParam);
            void ResetDraggedSplitter();
            void HandleResize(HWND hwnd, LPARAM lParam);
            void HandleDragging(HWND hwnd, LPARAM lParam);
            
    };

}
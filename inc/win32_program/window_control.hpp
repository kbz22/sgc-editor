#pragma once

#include "win32_program/windows_init.hpp"
#include "program/program.hpp"
#include "defaults.hpp"
#include <windows.h>

namespace win32_program
{

    struct SectionState {
        float layerHorizontalRatio = defaults::initialLayerHorizontalRatio;
        float layerVerticalRatio = defaults::initialLayerVerticalRatio;
        float tilesetRatio = defaults::initialTilesetRatio;

        bool draggingLayerHorizontal = false;
        bool draggingTileset = false;
        bool draggingLayerVertical = false;

        int layerWidth = static_cast<int>(defaults::WindowWidth * defaults::initialLayerHorizontalRatio);
        int layerHeight = static_cast<int>(defaults::WindowHeight * defaults::initialLayerVerticalRatio);
        int tilesetWidth = static_cast<int>(defaults::WindowWidth * defaults::initialTilesetRatio);

        int windowWidth = defaults::WindowWidth;
        int windowHeight = defaults::WindowHeight;

        int toolbarOffset = 0;        
    };

    SectionState& GetSectionState();

    void InitializeMenuControls(MainWindowContext& context);
    void CreateMainWindowContents(HWND hwnd, MainWindowContext& context, program::ProgramContext& programContext);

    bool CheckDragging(HWND hwnd, LPARAM lParam, MainWindowContext& context);
    void HandleResize(HWND hwnd, LPARAM lParam, MainWindowContext& context);
    void HandleDragging(HWND hwnd, LPARAM lParam, MainWindowContext& context);
    
}
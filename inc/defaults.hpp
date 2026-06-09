#pragma once

#include <string>
#include <string_view>

namespace defaults
{
    // Values
    constexpr int WindowWidth = 1600;
    constexpr int WindowHeight = 900;
    constexpr int minCollumnWidth = 150;
    constexpr int minCollumnHeight = 100;

    constexpr float initialLayerHorizontalRatio = 0.15f;
    constexpr float initialLayerVerticalRatio = 0.5f;
    constexpr float initialTilesetRatio = 0.2f;

    // Names
    const std::wstring_view WindowTitle = L"SGC Editor";
    const std::wstring_view NewFileTooltip = L"New File";

    // Assets
    const std::wstring_view ToolbarIconPath = L"./testicon2.png";
    
}
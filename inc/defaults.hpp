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
    constexpr int tileSize = 32;

    constexpr float initialLayerHorizontalRatio = 0.15f;
    constexpr float initialLayerVerticalRatio = 0.5f;
    constexpr float initialTilesetRatio = 0.2f;

    // Assets
    const std::wstring_view IconsPath = L"./toolbar_icons.png";
    const std::wstring_view DisabledIconsPath = L"./toolbar_icons_disabled.png";

    //File extensions
    const std::wstring_view MapFileExtension = L".sgcmap";
    const std::wstring_view PackageFileExtension = L".sgcp";
    const std::wstring_view TilesetFileExtension = L".sgctileset";
    
}
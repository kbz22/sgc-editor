#pragma once

#include <string>
#include <string_view>

namespace defaults
{
    constexpr int WindowWidth = 1600;
    constexpr int WindowHeight = 900;

    const std::wstring_view WindowTitle = L"SGC Editor";
    const std::wstring_view ToolbarIconPath = L"./testicon2.png";
    const std::wstring_view NewFileTooltip = L"New File";
}
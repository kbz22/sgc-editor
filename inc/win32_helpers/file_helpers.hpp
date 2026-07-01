#pragma once

#include <optional>
#include <filesystem>
#include <windows.h>

namespace win32_helpers {

    std::optional<std::filesystem::path> ShowSaveDialog(HWND ownerHwnd);
    std::optional<std::filesystem::path> ShowOpenDialog(HWND ownerHwnd);

}
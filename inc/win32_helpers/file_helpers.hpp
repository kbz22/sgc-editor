#pragma once

#include <optional>
#include <filesystem>
#include <string>
#include <windows.h>

namespace win32_helpers {

    std::optional<std::filesystem::path> ShowSaveDialog(HWND ownerHwnd, std::wstring fileType);
    std::optional<std::filesystem::path> ShowOpenDialog(HWND ownerHwnd, std::wstring fileType);

}
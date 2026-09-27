#pragma once

#include <optional>
#include <filesystem>
#include <string>
#include <vector>
#include <windows.h>

namespace win32_helpers {

    struct FileFilter
    {
        std::wstring name;
        std::vector<std::wstring> exts;
    };

    std::optional<std::filesystem::path> ShowSaveDialog(HWND ownerHwnd, const std::vector<FileFilter>& filters);
    std::optional<std::filesystem::path> ShowOpenDialog(HWND ownerHwnd, const std::vector<FileFilter>& filters);

    std::filesystem::path GetPreferencesDirectory();

}
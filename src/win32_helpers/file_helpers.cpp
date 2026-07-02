#include "win32_helpers/file_helpers.hpp"
#include <windows.h>
#include <commdlg.h>

std::optional<std::filesystem::path> win32_helpers::ShowSaveDialog(HWND owner, std::wstring fileType)
{
    wchar_t file[MAX_PATH] = {};
    std::wstring filter = L"Map File\0*." + fileType + L"\0All\0*.*\0";

    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner;
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = filter.c_str();
    ofn.lpstrDefExt = fileType.c_str();
    ofn.Flags = OFN_OVERWRITEPROMPT;

    if (GetSaveFileNameW(&ofn))
        return std::filesystem::path(file);

    return std::nullopt;
}

std::optional<std::filesystem::path> win32_helpers::ShowOpenDialog(HWND owner, std::wstring fileType)
{
    wchar_t file[MAX_PATH] = {};
    // std::wstring filter = L"Map File\0*." + fileType + L"\0All\0*.*\0";    

    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner;
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;    
    ofn.lpstrFilter = L"All Files (*.*)\0*.*\0\0";

    if (GetOpenFileNameW(&ofn))
        return std::filesystem::path(file);

    return std::nullopt;
}
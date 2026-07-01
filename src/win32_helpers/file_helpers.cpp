#include "win32_helpers/file_helpers.hpp"

std::optional<std::filesystem::path> win32_helpers::ShowSaveDialog(HWND owner)
{
    wchar_t file[MAX_PATH] = {};

    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner;
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"Map File\0*.sgmap\0All\0*.*\0";
    ofn.lpstrDefExt = L"sgmap";
    ofn.Flags = OFN_OVERWRITEPROMPT;

    if (GetSaveFileNameW(&ofn))
        return std::filesystem::path(file);

    return std::nullopt;
}

std::optional<std::filesystem::path> win32_helpers::ShowOpenDialog(HWND owner)
{
    wchar_t file[MAX_PATH] = {};

    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner;
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"Map File\0*.sgmap\0All\0*.*\0";

    if (GetOpenFileNameW(&ofn))
        return std::filesystem::path(file);

    return std::nullopt;
}
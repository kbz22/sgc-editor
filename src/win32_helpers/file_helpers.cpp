#include "win32_helpers/file_helpers.hpp"
#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>

std::wstring BuildFilter(const std::vector<win32_helpers::FileFilter>& filters)
{
    std::wstring result;

    for (const auto& filter : filters)
    {
        // Display name        
        result += filter.name;
        result.push_back(L'\0');

        // Pattern (*.ext;*.ext2)
        for (size_t i = 0; i < filter.exts.size(); ++i)
        {
            if (i > 0)
            {
                result += L";";
            }

            if (filter.exts[i] == L"*")
            {
                result += L"*.*";
            }
            else
            {
                result += L"*.";
                if (filter.exts[i].front() == L'.') {
                    result += filter.exts[i].substr(1); // Remove leading dot
                } else {
                    result += filter.exts[i];
                }
            }
        }

        result.push_back(L'\0');
    }

    // Final double-null terminator required by Win32
    result.push_back(L'\0');

    return result;
}

std::optional<std::filesystem::path> win32_helpers::ShowSaveDialog(HWND owner, const std::vector<FileFilter>& filters)
{
    wchar_t file[MAX_PATH] = {};
    std::wstring filter = BuildFilter(filters);

    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner;
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = filter.c_str();
    ofn.lpstrDefExt = filters.empty() ? nullptr : filters[0].exts.empty() ? nullptr : filters[0].exts[0].c_str();
    ofn.Flags = OFN_OVERWRITEPROMPT;

    if (GetSaveFileNameW(&ofn))
        return std::filesystem::path(file);

    return std::nullopt;
}

std::optional<std::filesystem::path> win32_helpers::ShowOpenDialog(HWND owner, const std::vector<FileFilter>& filters, int filterIndex)
{
    wchar_t file[MAX_PATH] = {};
    std::wstring filter = BuildFilter(filters);

    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner;
    ofn.lpstrFile = file;
    ofn.nMaxFile = MAX_PATH;    
    ofn.lpstrFilter = filter.c_str();
    ofn.nFilterIndex = filterIndex;

    if (GetOpenFileNameW(&ofn))
        return std::filesystem::path(file);

    return std::nullopt;
}

std::filesystem::path win32_helpers::GetPreferencesDirectory()
{
    PWSTR path = nullptr;

    HRESULT hr = SHGetKnownFolderPath(
        FOLDERID_RoamingAppData,
        KF_FLAG_DEFAULT,
        nullptr,
        &path
    );

    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to get Roaming AppData directory");
    }

    std::filesystem::path result(path);
    result /= "SGC";
    result /= "SGC Map Editor";

    CoTaskMemFree(path);

    std::filesystem::create_directories(result);

    return result;
}
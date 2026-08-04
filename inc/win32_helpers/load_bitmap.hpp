#include <windows.h>
#include <objidl.h> 
#include <wincodec.h>

#pragma comment(lib, "windowscodecs.lib")

#include <filesystem>
#include <span>

namespace win32_helpers {

    HBITMAP LoadPngWIC(std::filesystem::path const& path);
    HBITMAP LoadPngWIC(std::span<const std::byte> data);

    HBITMAP LoadBitmapFromResource(HINSTANCE hInstance, int resourceId);

}
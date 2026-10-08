#include <windows.h>
#include <objidl.h> 
#include <wincodec.h>

#include <filesystem>
#include <span>
#include <optional>

namespace win32_helpers 
{
    HBITMAP LoadPngWIC(std::span<const std::byte> data, double scale = 1.0);
    HBITMAP LoadBitmapFromResource(HINSTANCE hInstance, int resourceId, double scale = 1.0);

    void ComInitialize();
    void ComUninitialize();
}
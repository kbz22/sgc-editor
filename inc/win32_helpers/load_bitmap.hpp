#include <windows.h>
#include <objidl.h> 
#include <wincodec.h>

#pragma comment(lib, "windowscodecs.lib")

namespace win32_helpers {

    HBITMAP LoadPngWIC(const wchar_t* filename);

}
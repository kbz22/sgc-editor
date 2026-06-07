#include "win32_helpers/load_bitmap.hpp"

HBITMAP win32_helpers::LoadPngWIC(const wchar_t* filename)
{
    IWICImagingFactory* factory = nullptr;
    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICFormatConverter* converter = nullptr;
    HBITMAP hBmp = nullptr;

    // Initialize COM (if not already done)
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    // Create WIC factory
    CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&factory)
    );

    // Decode PNG
    factory->CreateDecoderFromFilename(
        filename,
        nullptr,
        GENERIC_READ,
        WICDecodeMetadataCacheOnLoad,
        &decoder
    );

    decoder->GetFrame(0, &frame);

    // Convert to 32bpp premultiplied BGRA (perfect for Win32)
    factory->CreateFormatConverter(&converter);
    converter->Initialize(
        frame,
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.0,
        WICBitmapPaletteTypeCustom
    );

    // Create HBITMAP
    UINT width, height;
    frame->GetSize(&width, &height);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -((int)height); // top-down bitmap
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HDC hdc = GetDC(nullptr);
    hBmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, hdc);

    converter->CopyPixels(nullptr, width * 4, width * height * 4, (BYTE*)bits);

    // Cleanup
    converter->Release();
    frame->Release();
    decoder->Release();
    factory->Release();

    return hBmp;
}
#include "win32_helpers/load_bitmap.hpp"
#include "program/except.hpp"

HBITMAP win32_helpers::LoadPngWIC(std::filesystem::path const& path)
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
        path.c_str(),
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

HBITMAP win32_helpers::LoadPngWIC(std::span<const std::byte> data)
{
    IWICImagingFactory* factory = nullptr;
    IWICStream* stream = nullptr;
    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICFormatConverter* converter = nullptr;
    HBITMAP hBmp = nullptr;

    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&factory)
    );

    factory->CreateStream(&stream);

    stream->InitializeFromMemory(
        reinterpret_cast<BYTE*>(
            const_cast<std::byte*>(data.data())
        ),
        static_cast<DWORD>(data.size())
    );

    factory->CreateDecoderFromStream(
        stream,
        nullptr,
        WICDecodeMetadataCacheOnLoad,
        &decoder
    );

    decoder->GetFrame(0, &frame);

    factory->CreateFormatConverter(&converter);

    converter->Initialize(
        frame,
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.0,
        WICBitmapPaletteTypeCustom
    );

    UINT width, height;
    frame->GetSize(&width, &height);

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -(int)height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;

    HDC hdc = GetDC(nullptr);
    hBmp = CreateDIBSection(
        hdc,
        &bmi,
        DIB_RGB_COLORS,
        &bits,
        nullptr,
        0
    );
    ReleaseDC(nullptr, hdc);

    converter->CopyPixels(
        nullptr,
        width * 4,
        width * height * 4,
        (BYTE*)bits
    );

    if (converter) converter->Release();
    if (frame) frame->Release();
    if (decoder) decoder->Release();
    if (stream) stream->Release();
    if (factory) factory->Release();

    return hBmp;
}

HBITMAP win32_helpers::LoadBitmapFromResource(HINSTANCE hInstance, int resourceId)
{
    HRSRC resource = FindResource(
        hInstance,
        MAKEINTRESOURCE(resourceId),
        RT_RCDATA
    );

    if (!resource)
    {
        return nullptr;
    }

    HGLOBAL loadedResource = LoadResource(
        hInstance,
        resource
    );

    if (!loadedResource)
    {
        return nullptr;
    }

    const auto* data = static_cast<const std::byte*>(
        LockResource(loadedResource)
    );

    if (!data)
    {
        return nullptr;
    }

    DWORD size = SizeofResource(
        hInstance,
        resource
    );

    return LoadPngWIC(
        std::span<const std::byte>(
            data,
            size
        )
    );
}
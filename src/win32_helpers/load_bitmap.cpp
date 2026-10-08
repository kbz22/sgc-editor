#include "win32_helpers/load_bitmap.hpp"
#include "program/except.hpp"
#include <wrl/client.h>

void win32_helpers::ComInitialize()
{
    auto result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    if(FAILED(result))
    {
        throw std::runtime_error("COM initialization for WIC failed.");
    }
}

void win32_helpers::ComUninitialize()
{
    CoUninitialize();
}

HBITMAP win32_helpers::LoadPngWIC(std::span<const std::byte> data, double scale)
{
    using Microsoft::WRL::ComPtr;

    ComPtr<IWICImagingFactory> factory;
    ComPtr<IWICStream> stream;
    ComPtr<IWICBitmapDecoder> decoder;
    ComPtr<IWICBitmapFrameDecode> frame;
    ComPtr<IWICFormatConverter> converter;
    HBITMAP hBmp;    

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
        stream.Get(),
        nullptr,
        WICDecodeMetadataCacheOnLoad,
        &decoder
    );

    decoder->GetFrame(0, &frame);

    UINT width, height;    
    frame->GetSize(&width, &height);  

    ComPtr<IWICBitmapSource> source = frame;
    ComPtr<IWICBitmapScaler> scaler;

    width *= scale;
    height *= scale;

    factory->CreateBitmapScaler(&scaler);
    scaler->Initialize(
        source.Get(),
        width,
        height,
        WICBitmapInterpolationModeHighQualityCubic
    );

    factory->CreateFormatConverter(&converter);

    converter->Initialize(
        frame.Get(),
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.0,
        WICBitmapPaletteTypeCustom
    );

      

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

    return hBmp;
}

HBITMAP win32_helpers::LoadBitmapFromResource(HINSTANCE hInstance, int resourceId, double scale)
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

    DWORD dataSize = SizeofResource(
        hInstance,
        resource
    );

    return LoadPngWIC(
        std::span<const std::byte>(
            data,
            dataSize
        ),
        scale
    );
}
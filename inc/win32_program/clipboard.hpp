#pragma once

#include <vector>
#include <string>
#include <span>
#include <cstdint>
#include <windows.h>

namespace win32_program {
    
    template<typename T>
    struct ClipboardSerializer{
        static constexpr std::wstring_view FormatName;
        static std::vector<uint8_t> Serialize(const T& object);
        static T Deserialize(std::span<const std::byte> data);
    };
    
    class Clipboard {
        public:
            template<typename T>
            static bool Set(const T &data)
            {
                const UINT format = GetFormat<T>();

                const std::vector<uint8_t> serialData = ClipboardSerializer<T>::Serialize(data);

                if (!OpenClipboard(nullptr)){
                    return false;
                }

                EmptyClipboard();

                HGLOBAL hMem = GlobalAlloc(
                    GMEM_MOVEABLE,
                    serialData.size()
                );

                if (!hMem)
                {
                    CloseClipboard();
                    return false;
                }

                void* destination = GlobalLock(hMem);

                if (!destination)
                {
                    GlobalFree(hMem);
                    CloseClipboard();
                    return false;
                }

                memcpy(destination, serialData.data(), serialData.size());
                GlobalUnlock(hMem);

                if (!SetClipboardData(format, hMem))
                {
                    GlobalFree(hMem);
                    CloseClipboard();
                    return false;
                }

                CloseClipboard();
                return true;
            }

            template<typename T>
            static UINT GetFormat()
            {
                static const UINT format = RegisterClipboardFormatW(ClipboardSerializer<T>::FormatName.data());

                return format;
            }

            template<typename T>
            static bool Get(T &data)
            {
                const UINT format = GetFormat<T>();

                if (!IsClipboardFormatAvailable(format))
                    return false;

                if (!OpenClipboard(nullptr))
                    return false;

                HANDLE handle = GetClipboardData(format);

                if (!handle)
                {
                    CloseClipboard();
                    return false;
                }

                const SIZE_T size = GlobalSize(handle);
                void* clipboardData = GlobalLock(handle);

                if (!clipboardData)
                {
                    CloseClipboard();
                    return false;
                }

                const std::span<const std::byte> bytes{
                    static_cast<const std::byte*>(clipboardData),
                    size
                };

                data = ClipboardSerializer<T>::Deserialize(bytes);

                GlobalUnlock(handle);
                CloseClipboard();

                return true;
            }            

            template<typename T>
            static bool Contains()
            {
                const UINT format = GetFormat<T>();

                return IsClipboardFormatAvailable(format) != FALSE;
            }

    };

}
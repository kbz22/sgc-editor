#include "debug.hpp"
#include <cstdio>
#include <windows.h>
#include <iterator>

void debug::DebugLog(const wchar_t *format, ...)
{
    wchar_t buffer[512];

    va_list args;
    va_start(args, format);
    _vsnwprintf_s(buffer, std::size(buffer), _TRUNCATE, format, args);
    va_end(args);

    OutputDebugStringW(buffer);
}
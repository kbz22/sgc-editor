#pragma once

#include <windows.h>
#include <commctrl.h>

#include <map>

#include "win32_program/win32_context.hpp"
#include "types.hpp"

namespace win32_program
{   

    void Init(HINSTANCE hInstance, Win32Context& context);        

    void Run();

    void SetupMenuBar(Win32Context& context);

    void SetupToolbar(Win32Context& context);

    void UpdateToolbar(Win32Context& context);

}

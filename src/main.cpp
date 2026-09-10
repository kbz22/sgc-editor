#include <windows.h>

#include "win32_program/windows_init.hpp"
#include "program/program.hpp"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    using namespace win32_program;    
    
    Init(hInstance);

    program::ProgramContext& programContext = program::GetProgramContext();
    programContext.StartDefault();

    ShowWindow(
        programContext.GetMainWindowHandle(),
        nCmdShow
    );

    Run();
    
    return 0;
}

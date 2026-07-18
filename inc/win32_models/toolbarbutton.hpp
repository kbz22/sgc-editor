#pragma once

#include "win32_program/windows_controls.hpp"
#include <string>

namespace win32_models {
    
    struct ToolbarButton
    {
        int imageIndex;
        win32_program::CommandId commandId;
        bool enabled = true;
        bool seperator = false;        
        bool grouped = false;
    };   

}
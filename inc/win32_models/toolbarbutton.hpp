#pragma once

#include "win32_program/windows_controls.hpp"
#include <string>

namespace win32_models {
    
    struct ToolbarButton
    {
        int imageIndex;
        bool enabled = true;
        bool seperator = false;
        bool grouped = false;
    };   

}
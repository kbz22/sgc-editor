#pragma once

#include "win32_program/control_setup.hpp"
#include <string>

namespace win32_models {

    struct ToolbarButton
    {
        int imageIndex;
        win32_program::CommandId commandId;
        std::wstring tooltip;

        bool enabled = true;
        bool seperator = false;
    };

}
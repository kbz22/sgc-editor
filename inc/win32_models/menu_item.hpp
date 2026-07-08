#pragma once

#include <vector>
#include <string>
#include <optional>
#include "win32_program/windows_controls.hpp"

namespace win32_models {

    struct MenuItem
    {
        std::wstring text;

        std::optional<win32_program::CommandId> command;

        bool enabled = true;
        bool checked = false;
        bool separator = false;

        std::vector<MenuItem> children;
    };

}
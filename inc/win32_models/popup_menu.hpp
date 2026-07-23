#pragma once

#include "action/action_types.hpp"
#include "locale/stringid.hpp"
#include <windows.h>

namespace win32_models {

    struct PopupMenu
    {
        HMENU hMenu;
        locale::StringId nameStringId;
        action::ActionType actionType;
    };

}
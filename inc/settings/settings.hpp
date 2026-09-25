#pragma once

#include "settings/shortcuts_setting.hpp"
#include "settings/auto_restore_files_setting.hpp"
#include "settings/default_open_filetype_setting.hpp"
 
namespace settings
{
    enum class Key : unsigned
    {
        Default,
        ShortcutsSetting,
        AutoRestoreFilesSetting,
        DefaultOpenFiletypeSetting
    };
}

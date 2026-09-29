#pragma once

namespace settings
{
    enum class SettingCategory
    {
        General = 0,
        Shortcuts,

        Count
    };
    
    enum class Key : unsigned
    {
        Default,
        ShortcutsSetting,
        AutoRestoreFilesSetting,
        DefaultOpenFiletypeSetting,

        Count
    };    
}
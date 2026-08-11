#pragma once

#include "locale/stringid.hpp"
#include <optional>

namespace action {

    enum class MenuId
    {
        None = -1,
        File = 0,
        Edit,
        Map,
        View,
        Help,

        Count
    };

    enum class GroupId
    {   
        Default,
        File,
        UndoRedo,
        LayerManagement,
        EditorLayerMode,
        EditorChunkMode,
        BrushMode,
        EraseMode,
        Zoom,
        GridMode
    };

    struct ActionDescription
    {
        int imageIndex = -1;
        int toolbarOrder = -1;
        int menuOrder = -1;

        bool checkGroupItem = false;

        GroupId groupId = GroupId::Default;
        MenuId menuId = MenuId::None;

        std::optional<locale::StringId> tooltipStringId = std::nullopt;
        locale::StringId nameStringId = locale::StringId::TextMissing;
    };

}
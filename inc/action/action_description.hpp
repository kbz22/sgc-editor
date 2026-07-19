#pragma once

#include "locale/stringid.hpp"

namespace action {

    enum class MenuId
    {
        None,
        File,
        Edit,
        Map,
        View
    };

    enum class GroupId
    {   
        Default,     
        File,
        UndoRedo,
        LayerManagement,
        EditorLayerMode,
        EditorChunkMode
    };

    struct ActionDescription
    {
        int imageIndex = -1;
        int toolbarOrder = -1;
        int menuOrder = -1;

        bool checkGroupItem = false;

        GroupId groupId = GroupId::File;
        MenuId menuId = MenuId::File;

        locale::StringId tooltipStringId = locale::StringId::TextMissing;
        locale::StringId nameStringId = locale::StringId::TextMissing;
    };

}
#pragma once

namespace action {

    enum class MenuId
    {
        None,
        File,
        Edit,
        Layer,
        View
    };

    enum class GroupId
    {        
        File,
        UndoRedo,
        LayerManagement,
        EditorLayerMode,
        EditorChunkMode
    };

    struct ActionDescription
    {
        int imageIndex = -1;

        bool toolbarItem = false;

        GroupId groupId = GroupId::File;
        MenuId menuId = MenuId::File;
    };

}
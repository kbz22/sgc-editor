#pragma once

#include "locale/command_info.hpp"
#include "win32_program/windows_controls.hpp"
#include <vector>

namespace locale {

    class CommandManager
    {
        private:
        std::vector<CommandInfo> m_commands = {
            { win32_program::CommandId::FileNew,  L"New file" },
            { win32_program::CommandId::FileOpen, L"Open file" },
            { win32_program::CommandId::FileSave, L"Save file" },
            { win32_program::CommandId::EditUndo, L"Undo" },
            { win32_program::CommandId::EditRedo, L"Redo" },
            { win32_program::CommandId::LayerAdd, L"Add layer" },
            { win32_program::CommandId::LayerRemove, L"Remove layer" },
            { win32_program::CommandId::LayerMoveUp, L"Move active layer up" },
            { win32_program::CommandId::LayerMoveDown, L"Move active layer down" },
            { win32_program::CommandId::EditorLayerModeNonActiveTransparent, L"Non-active layers transparent" },
            { win32_program::CommandId::EditorLayerModeSingleLayer, L"Single layer mode" },
            { win32_program::CommandId::EditorLayerModeSingleImage, L"Single image mode" },
            { win32_program::CommandId::EditorChunkModeFixedSize, L"Fixed size chunk mode" },
            { win32_program::CommandId::EditorChunkModeFree, L"Free chunk mode" }
        };

        public:
            const CommandInfo& Get(win32_program::CommandId id) const;

    
    };

}
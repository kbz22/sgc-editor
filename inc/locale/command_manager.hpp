#pragma once

#include "locale/command_info.hpp"
#include "locale/stringid.hpp"
#include "win32_program/windows_controls.hpp"
#include <vector>

namespace locale {

    class CommandManager
    {
        private:
        std::vector<CommandInfo> m_commands = {
            { win32_program::CommandId::FileNew,  StringId::TooltipFileNew },
            { win32_program::CommandId::FileOpen, StringId::TooltipFileOpen },
            { win32_program::CommandId::FileSave, StringId::TooltipFileSave },
            { win32_program::CommandId::EditUndo, StringId::TooltipEditUndo },
            { win32_program::CommandId::EditRedo, StringId::TooltipEditRedo },
            { win32_program::CommandId::LayerAdd, StringId::TooltipLayerAdd },
            { win32_program::CommandId::LayerRemove, StringId::TooltipLayerRemove },
            { win32_program::CommandId::LayerMoveUp, StringId::TooltipLayerMoveUp },
            { win32_program::CommandId::LayerMoveDown, StringId::TooltipLayerMoveDown },
            { win32_program::CommandId::EditorLayerModeNonActiveTransparent, StringId::TooltipEditorLayerModeNonActiveTransparent },
            { win32_program::CommandId::EditorLayerModeSingleLayer, StringId::TooltipEditorLayerModeSingleLayer },
            { win32_program::CommandId::EditorLayerModeSingleImage, StringId::TooltipEditorLayerModeSingleImage },
            { win32_program::CommandId::EditorChunkModeFixedSize, StringId::TooltipEditorChunkModeFixedSize },
            { win32_program::CommandId::EditorChunkModeFree, StringId::TooltipEditorChunkModeFree }
        };

        public:
            const CommandInfo& Get(win32_program::CommandId id) const;

    
    };

}
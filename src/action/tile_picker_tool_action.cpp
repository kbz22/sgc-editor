#include "action/tile_picker_tool_action.hpp"

action::TilePickerToolAction::TilePickerToolAction()
{
    m_actionType = ActionType::TilePickerTool;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = 36;
    m_actionDescription.toolbarOrder = 2800;
    m_actionDescription.menuOrder = 1000;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::ExtraEditTools;
    m_actionDescription.menuId = MenuId::Edit;
    m_actionDescription.tooltipStringId = locale::StringId::TooltipTilePicker;
    m_actionDescription.nameStringId = locale::StringId::NameTilePicker;
    m_actionDescription.shortcutContext = win32_program::ShortcutContext::MapEditor;
    m_actionDescription.shortcuts = {
        {win32_program::ShortcutModifier::Ctrl, win32_program::ShortcutKey::None}
    };
}

void action::TilePickerToolAction::Execute(program::ProgramContext& context)
{
    
}
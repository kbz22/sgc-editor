#include "action/symbol_change_group_action.hpp"
#include "program/program.hpp"

action::SymbolChangeGroupAction::SymbolChangeGroupAction()
{
    m_actionType = ActionType::NextSymbolGroup;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = -1;
    m_actionDescription.toolbarOrder = -1;
    m_actionDescription.menuOrder = 1200;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::SymbolTools;
    m_actionDescription.menuId = MenuId::Edit;
    m_actionDescription.tooltipStringId = std::nullopt;
    m_actionDescription.nameStringId = locale::StringId::NameSymbolNextGroup;
    m_actionDescription.shortcutContext = win32_program::ShortcutContext::MapEditor;
    m_actionDescription.shortcuts = {
        {win32_program::ShortcutModifier::Shift, win32_program::ShortcutKey::Tab}
    };
}

void action::SymbolChangeGroupAction::Execute(program::ProgramContext& context)
{
    auto tilesetSection = context.GetSection<sections::TilesetSection>();
    tilesetSection->NextSymbolGroup();
    tilesetSection->Update();
}
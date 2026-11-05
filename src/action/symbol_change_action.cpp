#include "action/symbol_change_action.hpp"
#include "program/program.hpp"

action::SymbolChangeAction::SymbolChangeAction()
{
    m_actionType = ActionType::NextSymbol;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = -1;
    m_actionDescription.toolbarOrder = -1;
    m_actionDescription.menuOrder = 1100;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::SymbolTools;
    m_actionDescription.menuId = MenuId::Edit;
    m_actionDescription.tooltipStringId = std::nullopt;
    m_actionDescription.nameStringId = locale::StringId::NameSymbolNextSymbol;
    m_actionDescription.shortcutContext = win32_program::ShortcutContext::MapEditor;
    m_actionDescription.shortcuts = {
        {win32_program::ShortcutModifier::None, win32_program::ShortcutKey::Tab}
    };
}

void action::SymbolChangeAction::Execute(program::ProgramContext& context)
{
    auto tilesetSection = context.GetSection<sections::TilesetSection>();
    tilesetSection->NextSymbol();
    tilesetSection->Update();
}
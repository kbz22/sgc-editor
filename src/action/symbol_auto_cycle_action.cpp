#include "action/symbol_auto_cycle_action.hpp"
#include "program/program.hpp"

action::SymbolAutoCycleAction::SymbolAutoCycleAction()
{
    m_actionType = ActionType::EnableAutomaticSymbolCycling;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = 42;
    m_actionDescription.toolbarOrder = 2900;
    m_actionDescription.menuOrder = 900;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::SymbolTools;
    m_actionDescription.menuId = MenuId::Edit;
    m_actionDescription.tooltipStringId = locale::StringId::TooltipSymbolCycling;
    m_actionDescription.nameStringId = locale::StringId::NameSymbolCycling;
}

void action::SymbolAutoCycleAction::Execute(program::ProgramContext& context)
{
    auto tilesetSection = context.GetSection<sections::TilesetSection>();
    auto toolbarSection = context.GetSection<sections::ToolbarSection>();
    bool newState = !tilesetSection->IsSymbolCycleThroughEnabled();

    tilesetSection->EnableSymbolCycleThrough(newState);
    SetChecked(newState);

    tilesetSection->Update();
    toolbarSection->Refresh(context);
    toolbarSection->Update();
}
#include "action/symbol_drawing_enable_action.hpp"
#include "program/program.hpp"
#include "sections/tileset_section.hpp"

action::SymbolDrawingEnableAction::SymbolDrawingEnableAction()
{
    m_actionType = ActionType::EnableSymbolDrawing;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = 40;
    m_actionDescription.toolbarOrder = 2800;
    m_actionDescription.menuOrder = 800;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::SymbolTools;
    m_actionDescription.menuId = MenuId::Edit;
    m_actionDescription.tooltipStringId = locale::StringId::TooltipEnableSymbol;
    m_actionDescription.nameStringId = locale::StringId::NameSymbolDrawing;
}

void action::SymbolDrawingEnableAction::Execute(program::ProgramContext& context)
{
    auto tilesetSection = context.GetSection<sections::TilesetSection>();
    auto toolbarSection = context.GetSection<sections::ToolbarSection>();
    bool newState = !tilesetSection->IsDrawingWithSymbols();

    tilesetSection->EnableSymbolDrawing(newState);
    SetChecked(newState);

    tilesetSection->Update();
    toolbarSection->Refresh(context);
    toolbarSection->Update();
}
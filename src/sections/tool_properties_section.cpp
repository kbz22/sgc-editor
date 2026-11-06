#include "sections/tool_properties_section.hpp"
#include "program/program.hpp"
#include "win32_models/toolbarbutton.hpp"
#include "win32_helpers/create_helpers.hpp"
#include <commctrl.h>

sections::ToolPropertiesSection::ToolPropertiesSection(program::ProgramContext &programContext) :
    m_mapSection{*programContext.GetSection<sections::MapSection>()},
    m_editorToolManager{*programContext.GetSection<sections::MapSection>()->GetEditorToolManager()}
{
    using namespace win32_program;
    using namespace win32_helpers;

    auto hMainWindow = programContext.GetMainWindowHandle();
    auto hInstance = programContext.GetHInstance();

    auto hwndRebar = win32_helpers::CreateRebar(
        hMainWindow,
        hInstance,
        static_cast<types::ctrid_t>(ControlId::ToolPropertiesRebar)
    );
    auto hwndToolbar = CreateToolbar(
        hwndRebar,
        hInstance,
        static_cast<types::ctrid_t>(ControlId::ToolPropertiesToolbar)
    );

    LONG style = static_cast<LONG>(SendMessage(hwndToolbar, TB_GETSTYLE, 0, 0)) | TBSTYLE_FLAT;
    SendMessage(hwndToolbar, TB_SETSTYLE, 0, style);

    SendMessage(hwndToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);

    SendMessage(hwndToolbar, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);        
    SendMessage(hwndToolbar, TB_SETIMAGELIST, 0, (LPARAM)programContext.GetImageList(program::ImageListType::Toolbar));
    SendMessage(hwndToolbar, TB_SETDISABLEDIMAGELIST, 0, (LPARAM)programContext.GetImageList(program::ImageListType::ToolbarDisabled));

    auto addSeperator = [](std::vector<TBBUTTON>& buttons) {
        TBBUTTON sepButton = {};
        sepButton.iBitmap = 0;
        sepButton.idCommand = static_cast<int>(action::ActionType::Default);
        sepButton.fsState = 0;
        sepButton.fsStyle = BTNS_SEP;
        buttons.push_back(sepButton);
    }; 
    
    auto toolbarActions = programContext.GetManager<action::ActionManager>()->GetToolbarActions();

    std::sort(toolbarActions.begin(), toolbarActions.end(), [](const action::Action* a, const action::Action* b) {
        return a->GetToolbarOrder() < b->GetToolbarOrder();
    });    

    std::vector<TBBUTTON> tbButtons;    
    action::GroupId lastGroupId = toolbarActions[0]->GetGroupId();

    for (const auto& action : toolbarActions)
    {
        if (action->GetGroupId() != lastGroupId) {
            addSeperator(tbButtons);            
        }

        TBBUTTON btn = {};

        if(action->IsWidget())
        {
            continue;
        }
        else
        {
            btn.iBitmap = action->GetToolbarImageIndex();
            btn.idCommand = static_cast<int>(action->GetType());
            btn.fsState = action->IsEnabled() ? TBSTATE_ENABLED : 0;
            btn.fsStyle = action->IsCheckGroupItem() ? BTNS_CHECK : btn.fsStyle;   
        }
        
        action::GroupId groupId = action->GetGroupId();
        lastGroupId = groupId;
        tbButtons.push_back(btn);
    }

    SendMessage(hwndToolbar, TB_ADDBUTTONS,
        (WPARAM)tbButtons.size(),
        (LPARAM)tbButtons.data()
    );

    auto dpiManager = programContext.GetManager<win32_program::DpiManager>();
    int buttonPadding = dpiManager->Scale(12);
    int buttonSize = dpiManager->Scale(32);
    int buttonBitmapSize = dpiManager->Scale(24);

    SendMessage(hwndToolbar, TB_SETPADDING, 0, MAKELPARAM(buttonPadding, 0));
    SendMessage(hwndToolbar, TB_SETBITMAPSIZE, 0, MAKELPARAM(buttonBitmapSize, buttonBitmapSize));
    SendMessage(hwndToolbar, TB_SETBUTTONSIZE, 0, MAKELPARAM(buttonSize, buttonSize));
    SendMessage(hwndToolbar, TB_AUTOSIZE, 0, 0);

    SIZE sz = {};
    SendMessage(hwndToolbar, TB_GETMAXSIZE, 0, (LPARAM)&sz);

    REBARBANDINFO rb = { sizeof(rb) };
    rb.fMask = RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE;
    rb.hwndChild = hwndToolbar;
    rb.cxMinChild = sz.cx;
    rb.cyMinChild = sz.cy;
    rb.fStyle = RBBS_CHILDEDGE;

    SendMessage(hwndRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rb);

    SetHwnd(hwndRebar, hMainWindow);
    m_hwndToolbar = hwndToolbar;

    SetupSubclass(hwndToolbar, this);
}

void sections::ToolPropertiesSection::Update()
{
    Redraw();
    return;
}

void sections::ToolPropertiesSection::HandleSectionResize()
{
    return;
}

void sections::ToolPropertiesSection::Refresh(program::ProgramContext& programContext)
{

}
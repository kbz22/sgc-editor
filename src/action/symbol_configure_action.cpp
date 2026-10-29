#include "action/symbol_configure_action.hpp"
#include "program/program.hpp"
#include "win32_models/symbol_tileset_control.hpp"
#include "symbol_config_dialog.h"
#include <windows.h>
#include <commdlg.h>
#include <sgc/coordinates/screenworld.hpp>

action::SymbolConfigureAction::SymbolConfigureAction()
{
    m_actionType = ActionType::SymbolConfiguration;
    m_enabled = true;
    m_checked = false;

    m_actionDescription.imageIndex = 39;
    m_actionDescription.toolbarOrder = 3000;
    m_actionDescription.menuOrder = 1000;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::SymbolTools;
    m_actionDescription.menuId = MenuId::Edit;
    m_actionDescription.tooltipStringId = locale::StringId::TooltipSymbolConfiguration;
    m_actionDescription.nameStringId = locale::StringId::NameSymbolConfiguration;
}

INT_PTR CALLBACK SymbolConfigureDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SymbolTilesetProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

void action::SymbolConfigureAction::Execute(program::ProgramContext& context)
{
    if(context.GetSection<sections::TilesetSection>() != nullptr) 
    {
        auto hMainWindow = context.GetMainWindowHandle();
        auto hInstance = context.GetHInstance();
        if(hMainWindow != nullptr)
        {
            auto tilesetViewControl = std::make_unique<win32_models::SymbolTilesetControl>(context);

            DialogBoxParam(
                hInstance,            
                MAKEINTRESOURCE(IDD_SYMBOL_CONFIG),
                hMainWindow,
                SymbolConfigureDialogProc,
                reinterpret_cast<LPARAM>(tilesetViewControl.get())
            );

            tilesetViewControl.reset();
        }
    }
}

INT_PTR CALLBACK SymbolConfigureDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto &programContext = program::GetProgramContext();

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            using namespace locale;
            using namespace win32_models;

            auto tilesetViewHwnd = GetDlgItem(hDlg, IDC_SYMBOL_TILESET_VIEW);
            auto tilesetViewControl = reinterpret_cast<SymbolTilesetControl*>(lParam);
            SetWindowLongPtr(hDlg, DWLP_USER, lParam);

            tilesetViewControl->Init(tilesetViewHwnd, programContext);
            tilesetViewControl->Refresh(programContext);
            tilesetViewControl->Update();

            tilesetViewControl->RegisterOnSizeChangedCallback([hDlg](sgc::tile::TileSize2D size)
            {
                std::wstring width = std::to_wstring(size.x);
                std::wstring height = std::to_wstring(size.y);
                
                SetDlgItemText(hDlg, IDC_SYMBOL_SIZE_X, width.c_str());
                SetDlgItemText(hDlg, IDC_SYMBOL_SIZE_Y, height.c_str());
            });

            SetDlgItemText(hDlg, IDC_SYMBOL_SIZE_X, L"0");
            SetDlgItemText(hDlg, IDC_SYMBOL_SIZE_Y, L"0");

            // localized text
            auto &stringLookup = programContext.GetStringLookup();
            auto symbolGroupName = stringLookup.Get(StringId::SymbolDialogSymbolGroupName).value_or(L"GROUP NAME");
            auto addName = stringLookup.Get(StringId::SymbolDialogAddName).value_or(L"ADD");
            auto groupSettingsName = stringLookup.Get(StringId::SymbolDialogGroupSettingsName).value_or(L"GROUP SETTINGS NAME");
            auto widthName = stringLookup.Get(StringId::SymbolDialogWidthName).value_or(L"WIDTH");
            auto heightName = stringLookup.Get(StringId::SymbolDialogHeightName).value_or(L"HEIGHT");
            auto saveName = stringLookup.Get(StringId::DialogSave).value_or(L"SAVE");
            auto cancelName = stringLookup.Get(StringId::DialogCancel).value_or(L"CANCEL");
            auto resetName = stringLookup.Get(StringId::SymbolDialogResetSymbols).value_or(L"RESET");

            SetDlgItemTextW(hDlg, IDOK, saveName.c_str());
            SetDlgItemTextW(hDlg, IDCANCEL, cancelName.c_str());
            SetDlgItemTextW(hDlg, IDC_SYMBOL_GROUP_LABEL, symbolGroupName.c_str());
            SetDlgItemTextW(hDlg, IDC_SYMBOL_ADD_GROUP, addName.c_str());
            SetDlgItemTextW(hDlg, IDC_SYMBOL_GROUP_CONFIG, groupSettingsName.c_str());
            SetDlgItemTextW(hDlg, IDC_SYMBOL_SIZE_X_LABEL, widthName.c_str());
            SetDlgItemTextW(hDlg, IDC_SYMBOL_SIZE_Y_LABEL, heightName.c_str());
            SetDlgItemTextW(hDlg, IDC_SYMBOL_RESTORE_DEFAULTS, resetName.c_str());

            auto title = stringLookup.Get(StringId::DialogTitleSymbolConfig).value_or(L"DIALOG TITLE");
            SetWindowTextW(hDlg, title.c_str());

            return TRUE;
        }

        case WM_COMMAND:
        {
            auto commandId = LOWORD(wParam);

            switch (commandId)
            {
                case IDOK:
                {
                    EndDialog(hDlg, IDCANCEL);
                    return TRUE;
                }

                case IDCANCEL:
                {
                    EndDialog(hDlg, IDCANCEL);
                    return TRUE;
                }

                case IDC_SYMBOL_RESTORE_DEFAULTS:
                {
                    auto tilesetViewControl = reinterpret_cast<win32_models::SymbolTilesetControl*>(
                        GetWindowLongPtr(hDlg, DWLP_USER)
                    );

                    tilesetViewControl->ResetCurrentView();
                    tilesetViewControl->SetSize({1,1});
                    tilesetViewControl->Update();
                    return TRUE;
                }

                default:
                {
                    return TRUE;
                }
            }            
        }
    }

    return FALSE;
}

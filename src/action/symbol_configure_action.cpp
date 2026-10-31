#include "action/symbol_configure_action.hpp"
#include "program/program.hpp"
#include "win32_models/symbol_tileset_control.hpp"
#include "symbol_config_dialog.h"
#include "textbox_dialog.h"
#include <windows.h>
#include <commdlg.h>
#include <sgc/coordinates/screenworld.hpp>
#include <algorithm>

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
LRESULT CALLBACK AddGroupDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

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
    using PositionsPointer = std::unique_ptr<std::vector<sgc::tile::TilePosition2D>>;
    static std::unordered_map<std::wstring, PositionsPointer> symbolGroups{};
    auto &programContext = program::GetProgramContext();

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            using namespace locale;
            using namespace win32_models;

            symbolGroups.clear();

            auto tilesetViewHwnd = GetDlgItem(hDlg, IDC_SYMBOL_TILESET_VIEW);
            auto tilesetViewControl = reinterpret_cast<SymbolTilesetControl*>(lParam);
            SetWindowLongPtr(hDlg, DWLP_USER, lParam);

            auto defaultSymbolSize = sgc::tile::TileSize2D{1,1};

            tilesetViewControl->RegisterOnSizeChangedCallback([hDlg](sgc::tile::TileSize2D size)
            {
                std::wstring width = std::to_wstring(size.x);
                std::wstring height = std::to_wstring(size.y);
                
                SetDlgItemText(hDlg, IDC_SYMBOL_SIZE_X, width.c_str());
                SetDlgItemText(hDlg, IDC_SYMBOL_SIZE_Y, height.c_str());
            });

            tilesetViewControl->Init(tilesetViewHwnd, programContext);
            tilesetViewControl->SetSymbolSize(defaultSymbolSize);
            tilesetViewControl->Refresh(programContext);
            tilesetViewControl->Update();

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
            auto newName = stringLookup.Get(StringId::SymbolDialogNewGroup).value_or(L"NEW");

            SetDlgItemTextW(hDlg, IDOK, saveName.c_str());
            SetDlgItemTextW(hDlg, IDCANCEL, cancelName.c_str());
            SetDlgItemTextW(hDlg, IDC_SYMBOL_GROUP_LABEL, symbolGroupName.c_str());
            SetDlgItemTextW(hDlg, IDC_SYMBOL_SAVE_GROUP, addName.c_str());
            SetDlgItemTextW(hDlg, IDC_SYMBOL_GROUP_CONFIG, groupSettingsName.c_str());
            SetDlgItemTextW(hDlg, IDC_SYMBOL_SIZE_X_LABEL, widthName.c_str());
            SetDlgItemTextW(hDlg, IDC_SYMBOL_SIZE_Y_LABEL, heightName.c_str());
            SetDlgItemTextW(hDlg, IDC_SYMBOL_RESTORE_DEFAULTS, resetName.c_str());
            SetDlgItemTextW(hDlg, IDC_SYMBOL_NEW_GROUP, newName.c_str());

            auto title = stringLookup.Get(StringId::DialogTitleSymbolConfig).value_or(L"DIALOG TITLE");
            SetWindowTextW(hDlg, title.c_str());

            return TRUE;
        }

        case WM_COMMAND:
        {
            auto commandId = LOWORD(wParam);

            auto handleUserResize = [hDlg](int value, bool isX)
            {
                auto tilesetViewControl = reinterpret_cast<win32_models::SymbolTilesetControl*>(
                    GetWindowLongPtr(hDlg, DWLP_USER)
                );

                auto currentSize = tilesetViewControl->GetSymbolSize();
                auto maxSize = tilesetViewControl->GetMaxSymbolSize();

                value = static_cast<int>(std::min(
                    static_cast<sgc::math::ival>(value),
                    isX ? maxSize.x : maxSize.y
                ));

                tilesetViewControl->SetSymbolSize({
                    isX  ? value : currentSize.x,
                    !isX ? value : currentSize.y
                });
                tilesetViewControl->Update();
            };

            switch (commandId)
            {
                case IDOK:
                {
                    HWND focused = GetFocus();

                    if (focused == GetDlgItem(hDlg, IDC_SYMBOL_SIZE_X) || focused == GetDlgItem(hDlg, IDC_SYMBOL_SIZE_Y))
                    {                        
                        SetFocus(hDlg);
                    }
                    else
                    {
                        EndDialog(hDlg, IDCANCEL);
                    }
                    
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
                    tilesetViewControl->SetSymbolSize({1,1});
                    tilesetViewControl->Update();
                    return TRUE;
                }

                case IDC_SYMBOL_SIZE_X:
                {
                    if (HIWORD(wParam) == EN_KILLFOCUS)
                    {
                        wchar_t buffer[32];
                        GetDlgItemText(hDlg, IDC_SYMBOL_SIZE_X, buffer, 32);

                        try
                        {
                            int value = std::stoi(buffer);
                            handleUserResize(value, true);
                        }
                        catch (std::exception const&)
                        {
                            // Invalid input
                        }
                    }
                    return TRUE;
                }

                case IDC_SYMBOL_SIZE_Y:
                {
                    if (HIWORD(wParam) == EN_KILLFOCUS)
                    {
                        wchar_t buffer[32];
                        GetDlgItemText(hDlg, IDC_SYMBOL_SIZE_Y, buffer, 32);

                        try
                        {
                            int value = std::stoi(buffer);
                            handleUserResize(value, false);
                        }
                        catch (std::exception const&)
                        {
                            // Invalid input
                        }
                    }
                    return TRUE;
                }

                case IDC_SYMBOL_SAVE_GROUP:
                {
                    using namespace locale;

                    auto tilesetViewControl = reinterpret_cast<win32_models::SymbolTilesetControl*>(
                        GetWindowLongPtr(hDlg, DWLP_USER)
                    );
                    auto hInstance = programContext.GetHInstance();
                    auto &stringLookup = programContext.GetStringLookup();

                    auto errorMsg = stringLookup.Get(StringId::UserErrorSymbolGroupEmpty).value_or(L"GROUP EMPTY");
                    auto errorName = stringLookup.Get(StringId::ErrorName).value_or(L"ERROR");

                    if(tilesetViewControl->Empty())
                    {
                        MessageBox(hDlg, errorMsg.c_str(), errorName.c_str(), MB_OK | MB_ICONERROR);   
                        return TRUE;
                    }

                    std::wstring groupName;
                    auto result = DialogBoxParam(
                        hInstance,
                        MAKEINTRESOURCE(IDD_TEXTBOX_DIALOG),
                        hDlg,
                        AddGroupDialogProc,
                        reinterpret_cast<LPARAM>(&groupName)
                    );

                    if(result == IDOK)
                    {
                        if(symbolGroups.contains(groupName))
                        {
                            auto wrnTitle = stringLookup.Get(StringId::WarningSymbolGroupAlreadyExistsTitle).value_or(L"OVERWRITE MSG");
                            auto wrnMsg = stringLookup.Get(StringId::WarningSymbolGroupAlreadyExistsMessage).value_or(L"WARNING");

                            result = MessageBox(
                                hDlg,
                                wrnMsg.c_str(),
                                wrnTitle.c_str(),
                                MB_YESNO | MB_ICONQUESTION
                            );
                        }
                        else
                        {
                            result = IDYES;
                        }

                        if(result == IDYES)
                        {
                            auto positions = tilesetViewControl->ResetCurrentView();
                            symbolGroups[groupName] = std::make_unique<std::vector<sgc::tile::TilePosition2D>>(positions);

                            auto groupCombo = GetDlgItem(hDlg, IDC_SYMBOL_GROUP_COMBO);
                            
                            for (const auto &[_groupName, _positions] : symbolGroups)
                            {
                                auto index = SendMessage(groupCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(_groupName.c_str()));
                                SendMessage(groupCombo, CB_SETITEMDATA, index, reinterpret_cast<LPARAM>(_positions.get()));
                            }

                            tilesetViewControl->Update();
                        }
                    }

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

LRESULT CALLBACK AddGroupDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_INITDIALOG:
        {
            using namespace locale;

            auto &programContext = program::GetProgramContext();
            
            auto stringLookup = programContext.GetStringLookup();
            auto createStr = stringLookup.Get(StringId::SymbolDialogAddName).value_or(L"ADD NAME"); 
            auto cancelStr = stringLookup.Get(StringId::DialogCancel).value_or(L"CANCEL NAME");
            auto nameStr = stringLookup.Get(StringId::NewDialogNameName).value_or(L"NAME NAME");
            auto title = stringLookup.Get(StringId::DialogTitleNewSymbolGroup).value_or(L"DIALOG TITLE");

            SetDlgItemTextW(hDlg, IDOK, createStr.c_str());
            SetDlgItemTextW(hDlg, IDCANCEL, cancelStr.c_str());
            SetDlgItemTextW(hDlg, IDC_TEXTBOX_LABEL_NAME, nameStr.c_str());

            SetWindowTextW(hDlg, title.c_str());

            SetWindowLongPtr(hDlg, DWLP_USER, lParam);

            return TRUE;
        }

        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case IDOK:
                {
                    auto &programContext = program::GetProgramContext();
                    auto groupName = reinterpret_cast<std::wstring*>(
                        GetWindowLongPtr(hDlg, DWLP_USER)
                    );
                    int length = GetWindowTextLength(
                        GetDlgItem(hDlg, IDC_TEXTBOX_NAME)
                    );

                    groupName->resize(length);
                    GetDlgItemText(hDlg, IDC_TEXTBOX_NAME, groupName->data(), length + 1);

                    auto stringLookup = programContext.GetStringLookup();
                    auto errorName = stringLookup.Get(locale::StringId::ErrorName).value_or(L"ERROR NAME");
                    auto noNameStr = stringLookup.Get(locale::StringId::UserErrorNoSymbolGroupName).value_or(L"NO PACKAGE MESSAGE");

                    if(groupName->empty()) {
                        MessageBox(hDlg, noNameStr.c_str(), errorName.c_str(), MB_OK | MB_ICONERROR);
                        return TRUE;
                    }
                    
                    EndDialog(hDlg, IDOK);
                    return TRUE;
                }

                case IDCANCEL:
                    EndDialog(hDlg, IDCANCEL);
                    return TRUE;
            }
        }
    }

    return FALSE;
}
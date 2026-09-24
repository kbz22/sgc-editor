#include "action/settings_action.hpp"
#include "program/program.hpp"
#include "settings/settings_manager.hpp"
#include "settings_dialog.h"

action::SettingsAction::SettingsAction()
{
    m_actionType = ActionType::Settings;
    m_enabled = true;
    m_checked = false;
    
    m_actionDescription.imageIndex = 3;
    m_actionDescription.toolbarOrder = -1; //!idk
    m_actionDescription.menuOrder = 900;
    m_actionDescription.checkGroupItem = false;
    m_actionDescription.groupId = GroupId::Preferences;
    m_actionDescription.menuId = MenuId::File;
    m_actionDescription.tooltipStringId = locale::StringId::TooltipSettings;
    m_actionDescription.nameStringId = locale::StringId::NameSettings;
}

INT_PTR CALLBACK SettingsDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

void action::SettingsAction::Execute(program::ProgramContext& context)
{
    auto hMainWindow = context.GetMainWindowHandle();
    if(hMainWindow != nullptr){
        DialogBox(
            context.GetHInstance(),            
            MAKEINTRESOURCE(IDD_SETTINGS),
            hMainWindow,
            SettingsDialogProc
        );
    }
}

INT_PTR CALLBACK GeneralSettingsDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ShortcutsSettingsDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

INT_PTR CALLBACK SettingsDialogProc(HWND hDlg, UINT msg, WPARAM wParam, [[maybe_unused]] LPARAM lParam)
{
    using namespace win32_program;
    using namespace settings;

    program::ProgramContext& context = program::GetProgramContext();
    auto settingsManager = context.GetManager<SettingsManager>();

    auto changePage = [&settingsManager]() {
        auto currentCategory = settingsManager->GetCurrentCategory();
        auto hwndCurrentCategory = settingsManager->GetCategoryWindow(currentCategory);

        for (int i = 0; i < static_cast<int>(SettingCategory::Count); ++i)
        {
            auto category = static_cast<SettingCategory>(i);
            auto hwndCategory = settingsManager->GetCategoryWindow(category);
            if (hwndCategory != nullptr)
            {                    
                ShowWindow(
                    hwndCategory,
                    hwndCurrentCategory == hwndCategory ? SW_SHOW : SW_HIDE
                );
            }
        }
    };

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            settingsManager->SetCategoryWindow(
                SettingCategory::General, 
                CreateDialog(
                    context.GetHInstance(),
                    MAKEINTRESOURCE(IDD_SETTINGS_GENERAL),
                    hDlg,
                    GeneralSettingsDialogProc
                )
            );
            settingsManager->SetCategoryWindow(
                SettingCategory::Shortcuts, 
                CreateDialog(
                    context.GetHInstance(),
                    MAKEINTRESOURCE(IDD_SETTINGS_SHORTCUTS),
                    hDlg,
                    ShortcutsSettingsDialogProc
                )
            );

            settingsManager->SetCategory(SettingCategory::General);

            changePage();

            auto list = GetDlgItem(hDlg, IDC_SETTINGS_LIST);
            auto stringLookup = &context.GetStringLookup();
            auto addItem = [list, &settingsManager](std::wstring text, SettingCategory category)
            {
                TVINSERTSTRUCT item{};
                item.hParent = TVI_ROOT;
                item.hInsertAfter = TVI_LAST;
                item.item.mask = TVIF_TEXT | TVIF_PARAM;
                item.item.pszText = const_cast<LPWSTR>(text.c_str());
                item.item.lParam = static_cast<LPARAM>(category);
                TreeView_InsertItem(list, &item);
            };

            addItem(stringLookup->Get(locale::StringId::SettingsNameGeneral).value_or(L"1"), SettingCategory::General);
            addItem(stringLookup->Get(locale::StringId::SettingsNameShortcuts).value_or(L"2"), SettingCategory::Shortcuts);

            return TRUE;
        }

        case WM_COMMAND:
        {
            auto commandId = LOWORD(wParam);

            switch (commandId)
            {
                case IDOK:
                {
                    EndDialog(hDlg, IDOK);
                    return TRUE;
                }

                case IDCANCEL:
                {
                    EndDialog(hDlg, IDCANCEL);
                    return TRUE;
                }
            }
            break;
        }

         case WM_NOTIFY:
        {
            auto* notification = reinterpret_cast<LPNMTREEVIEW>(lParam);

            if (notification->hdr.idFrom == IDC_SETTINGS_LIST)
            {
                switch (notification->hdr.code)
                {
                    case TVN_SELCHANGED:
                    {
                        auto category = static_cast<int>(notification->itemNew.lParam);
                        settingsManager->SetCategory(static_cast<SettingCategory>(category));
                        changePage();
                        return TRUE;
                    }
                }
            }

            break;
        }
    }

    return FALSE;
}
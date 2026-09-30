#include "settings_dialog.h"
#include "program/program.hpp"
#include "settings/shortcut_settings_manager.hpp"
#include "settings/settings.hpp"
#include "locale/shortcut_to_string.hpp"
#include "win32_helpers/file_helpers.hpp"

#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <algorithm>
#include <cwctype>

INT_PTR CALLBACK GeneralSettingsDialogProc(HWND hDlg, UINT msg, WPARAM wParam, [[maybe_unsused]] LPARAM lParam)
{
    auto &programContext = program::GetProgramContext();
    auto settingManager = programContext.GetManager<settings::SettingsManager>();
    auto defaultOpenFiletypeSetting = settingManager->GetSetting<settings::DefaultOpenFiletypeSetting>();
    auto autoRestoreFilesSetting = settingManager->GetSetting<settings::AutoRestoreFilesSetting>();

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            // Combo box for selecting the default open file type
            auto stringLookup = programContext.GetStringLookup();            
            auto mapFilesString = stringLookup.Get(locale::StringId::NameMapFile).value_or(L"MAP FILE");
            auto tilesetFilesString = stringLookup.Get(locale::StringId::NameTilesetFile).value_or(L"TILESET FILE");
            auto packageFilesString = stringLookup.Get(locale::StringId::NamePackageFile).value_or(L"PACKAGE FILE");
            auto startupNameString = stringLookup.Get(locale::StringId::SettingsGeneralStartupName).value_or(L"STARTUP NAME");
            auto fileNameString = stringLookup.Get(locale::StringId::SettingsGeneralFileName).value_or(L"FILE NAME");
            auto autorestoreString = stringLookup.Get(locale::StringId::SettingsGeneralAutoRestoreOpenFiles).value_or(L"AUTO RESTORE TEXT");
            auto defaultOpenString = stringLookup.Get(locale::StringId::SettingsGeneralDefaultOpenFileType).value_or(L"GENERAL DEFAULT");

            SetDlgItemTextW(hDlg, IDC_SETTINGS_STARTUP_NAME, startupNameString.c_str());
            SetDlgItemTextW(hDlg, IDC_SETTINGS_FILE_NAME, fileNameString.c_str());
            SetDlgItemTextW(hDlg, IDC_SETTINGS_RESTORE_SESSION, autorestoreString.c_str());
            SetDlgItemTextW(hDlg, IDC_SETTINGS_DEF_OPEN_FILE_TEXT, defaultOpenString.c_str());

            std::vector<std::wstring> filters = {
                mapFilesString,
                tilesetFilesString,
                packageFilesString
            };

            auto defaultOpenComboBox = GetDlgItem(hDlg, IDC_SETTINGS_DEFAULT_FILE_TYPE);

            for(const auto &filter : filters)
            {
                SendMessage(defaultOpenComboBox, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(filter.c_str()));
            }

            auto defaultOpenFiletype = defaultOpenFiletypeSetting->GetValue();
            SendMessage(defaultOpenComboBox, CB_SETCURSEL, static_cast<WPARAM>(defaultOpenFiletype), 0);

            // Checkbox for automatically restoring session
            auto autoRestoreCheckbox = GetDlgItem(hDlg, IDC_SETTINGS_RESTORE_SESSION);
            SendMessage(autoRestoreCheckbox, BM_SETCHECK, static_cast<WPARAM>(autoRestoreFilesSetting->GetValue() ? BST_CHECKED : BST_UNCHECKED), 0);

            return TRUE;
        }

        case WM_COMMAND:
        {
            auto commandId = LOWORD(wParam);

            switch(commandId)
            {
                case IDC_SETTINGS_DEFAULT_FILE_TYPE:
                {
                    if(HIWORD(wParam) == CBN_SELCHANGE)
                    {
                        auto defaultOpenComboBox = GetDlgItem(hDlg, IDC_SETTINGS_DEFAULT_FILE_TYPE);
                        auto selectedIndex = static_cast<int>(SendMessage(defaultOpenComboBox, CB_GETCURSEL, 0, 0));
                        defaultOpenFiletypeSetting->SetValue(static_cast<file::FileType>(selectedIndex));
                    }
                    break;
                }

                case IDC_SETTINGS_RESTORE_SESSION:
                {
                    if(HIWORD(wParam) == BN_CLICKED)
                    {
                        auto autoRestoreCheckbox = GetDlgItem(hDlg, IDC_SETTINGS_RESTORE_SESSION);
                        auto isChecked = SendMessage(autoRestoreCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED;
                        autoRestoreFilesSetting->SetValue(isChecked);
                    }
                    break;
                }
            }

            break;
        }
    }
    return FALSE;
}

LRESULT CALLBACK ShortcutsSettingsListViewProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, [[maybe_unused]]UINT_PTR id, DWORD_PTR data)
{
    using namespace settings;
    
    switch (msg)
    {
        case WM_GETDLGCODE:
        {
            return DLGC_WANTALLKEYS;
        }

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            auto &programContext = *reinterpret_cast<program::ProgramContext *>(data);
            auto shortcutSetting = programContext.GetManager<settings::SettingsManager>()->GetSetting<settings::ShortcutsSetting>();
            auto &shortcutSettingsManager = shortcutSetting->GetShortcutSettingsManager();

            if(wparam == VK_ESCAPE)
            {
                shortcutSettingsManager.CancelEdit();
                return 0;
            }            

            if(shortcutSettingsManager.IsEditing())
            {
                win32_program::Shortcut newShortcut;
                newShortcut.modifier = win32_program::ShortcutManager::GetShortcutModifierFromKeyState();
                newShortcut.key = static_cast<UINT>(wparam);

                shortcutSettingsManager.SetEditedShortcut(
                    newShortcut, 
                    programContext
                );

                return 0;
            }                

            break;
        }

        case WM_CHAR:
        case WM_SYSCHAR:
        {
            auto &programContext = *reinterpret_cast<program::ProgramContext *>(data);
            auto shortcutSetting = programContext.GetManager<settings::SettingsManager>()->GetSetting<settings::ShortcutsSetting>();
            auto &shortcutSettingsManager = shortcutSetting->GetShortcutSettingsManager();

            if(shortcutSettingsManager.IsEditing())
            {
                return 0;
            }

            break;
        }
    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

constexpr int gc_TimerId = 2;

INT_PTR CALLBACK ShortcutsSettingsDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    using namespace settings;

    auto &programContext = program::GetProgramContext();
    auto shortcutSetting = programContext.GetManager<settings::SettingsManager>()->GetSetting<settings::ShortcutsSetting>();
    auto &shortcutSettingsManager = shortcutSetting->GetShortcutSettingsManager();

    auto refreshShortcutList = [&programContext, &shortcutSettingsManager, hDlg](std::wstring filter = L"")
    {
        HWND listView = GetDlgItem(hDlg, IDC_SHORTCUT_LIST);

        ListView_DeleteAllItems(listView);

        auto &stringLookup = programContext.GetStringLookup();
        auto &shortcutManager = *programContext.GetManager<win32_program::ShortcutManager>();
        auto allActions = programContext.GetManager<action::ActionManager>()->GetActions();

        for(auto &action : allActions)
        {
            if(action->IsPopup())
                continue;

            auto actionStringId = action->GetNameStringId();
            auto shortcutStringId = action->GetShortcutStringId();
            std::wstring actionName;

            if(shortcutStringId.has_value()){
                actionName = stringLookup.Get(shortcutStringId.value()).value_or(L"");
            }
            else if(actionStringId.has_value()){
                actionName = stringLookup.Get(actionStringId.value()).value_or(L"");
            }
            else continue;

            if(actionName.empty())
                continue;

            auto shortcuts = shortcutManager.GetShortcutsForAction(action->GetType());                

            auto insertItem = [&action, &actionName, &listView, &programContext](win32_program::Shortcut shortcut, ShortcutSettingsManager &shortcutSettingsManager)
            {
                ShortcutEntry entry{};
                entry.index = ListView_GetItemCount(listView);
                entry.shortcut = shortcut;
                entry.actionType = action->GetType();
                entry.context = action->GetShortcutContext();
                entry.actionNameString = actionName;
                entry.shortcutString = locale::ShortcutToString(shortcut, programContext);

                shortcutSettingsManager.AddShortcutEntry(entry);

                LVITEM item{};
                item.mask = LVIF_TEXT | LVIF_PARAM;
                item.lParam = static_cast<LPARAM>(entry.actionType);
                item.pszText = const_cast<LPWSTR>(actionName.c_str());
                item.iItem = entry.index;
                ListView_InsertItem(listView, &item);

                ListView_SetItemText(
                    listView,
                    item.iItem,
                    1,
                    const_cast<LPWSTR>(entry.shortcutString.c_str())
                );
            };

            std::transform(filter.begin(), filter.end(), filter.begin(), [](wchar_t c){ return std::towlower(c); });
            auto actionNameLower = actionName;
            std::transform(actionNameLower.begin(), actionNameLower.end(), actionNameLower.begin(), [](wchar_t c){ return std::towlower(c); });            

            if(shortcuts.empty())
            {
                if(actionNameLower.find(filter) != std::wstring::npos)
                    insertItem(win32_program::Shortcut{}, shortcutSettingsManager);
            }

            for(auto &shortcut : shortcuts)
            {
                if(actionNameLower.find(filter) != std::wstring::npos)
                    insertItem(shortcut, shortcutSettingsManager);
                else
                {
                    auto shortcutLower = locale::ShortcutToString(shortcut, programContext);
                    std::transform(shortcutLower.begin(), shortcutLower.end(), shortcutLower.begin(), [](wchar_t c){ return std::towlower(c); });
                    if(shortcutLower.find(filter) != std::wstring::npos)
                        insertItem(shortcut, shortcutSettingsManager);
                }
            }
        }
    };

    switch (msg)
    {
        case WM_INITDIALOG:
        {   
            // set up localized strings
            auto &stringLookup = programContext.GetStringLookup();
            auto filterLabel = stringLookup.Get(locale::StringId::SettingsShortcutFilter).value_or(L"FILTER NAME");

            SetDlgItemTextW(hDlg, IDC_SHORTCUT_FILTER_NAME, filterLabel.c_str());

            auto listView = GetDlgItem(hDlg, IDC_SHORTCUT_LIST);
            constexpr int c_actionColumnWidth = 130;
            constexpr int c_shortcutColumnWidth = 2 * c_actionColumnWidth;           
            
            SetWindowSubclass(
                listView,
                ShortcutsSettingsListViewProc,
                1,
                reinterpret_cast<DWORD_PTR>(&programContext)
            );

            ListView_SetExtendedListViewStyle(
                listView,
                LVS_EX_FULLROWSELECT |
                LVS_EX_DOUBLEBUFFER |
                LVS_EX_HEADERDRAGDROP
            );
            
            auto actionName = stringLookup.Get(locale::StringId::SettingsShortcutActionName).value_or(L"ACTION NAME");
            auto shortcutName = stringLookup.Get(locale::StringId::SettingsShortcutShortcutName).value_or(L"SHROTCUT NAME");
            
            LVCOLUMN column{};

            column.mask = LVCF_TEXT | LVCF_WIDTH;

            column.pszText = const_cast<LPWSTR>(actionName.c_str());
            column.cx = c_actionColumnWidth;
            ListView_InsertColumn(listView, 0, &column);

            column.pszText = const_cast<LPWSTR>(shortcutName.c_str());
            column.cx = c_shortcutColumnWidth;
            ListView_InsertColumn(listView, 1, &column);

            refreshShortcutList();

            return TRUE;
        }

        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_LBUTTONDOWN:
        {
            shortcutSettingsManager.CancelEdit();
            break;
        }

        case WM_NOTIFY:
        {
            auto* notification = reinterpret_cast<NMHDR*>(lParam);

            if (notification->idFrom == IDC_SHORTCUT_LIST)
            {
                switch (notification->code)
                {
                    case NM_DBLCLK:
                    {
                        auto* info = reinterpret_cast<NMITEMACTIVATE*>(lParam);
                        auto listView = GetDlgItem(hDlg, IDC_SHORTCUT_LIST);

                        LVITEM item{};
                        item.mask = LVIF_PARAM;
                        item.iItem = info->iItem;                        

                        ListView_GetItem(listView, &item);

                        auto &stringLookup = programContext.GetStringLookup();
                        auto str = stringLookup.Get(locale::StringId::SettingsShortcutSetNew).value_or(L"");

                        ListView_SetItemText(
                            listView,
                            item.iItem,
                            1,
                            const_cast<LPWSTR>(str.c_str())
                        );

                        auto shortcutEntryPtr = shortcutSettingsManager.GetShortcutEntry(item.iItem);
                        auto shortcutEntry = *shortcutEntryPtr; // copy!!
                        shortcutEntry.shortcut = {};

                        shortcutSettingsManager.SetEditedShortcutEntry(shortcutEntry);

                        SetTimer(hDlg, gc_TimerId, 2800, nullptr);
                        
                        break;
                    }
                }
            }            
            break;
        }

        case WM_TIMER:
        {
            if (wParam == gc_TimerId)
            {
                KillTimer(hDlg, gc_TimerId);
                auto editedShortcut = shortcutSettingsManager.GetEditedShortcut();

                auto listView = GetDlgItem(hDlg, IDC_SHORTCUT_LIST);

                ListView_SetItemText(
                    listView,
                    editedShortcut->index,
                    1,
                    const_cast<LPWSTR>(editedShortcut->shortcutString.c_str())
                );
            }
            break;
        }

        case WM_COMMAND:
        {
            if (LOWORD(wParam) == IDC_SHORTCUT_FILTER && HIWORD(wParam) == EN_CHANGE)
            {
                auto filterElement = GetDlgItem(hDlg, IDC_SHORTCUT_FILTER);
                auto filterTextLength = GetWindowTextLengthW(filterElement);

                std::wstring filterText(filterTextLength, L'\0');
                GetWindowText(filterElement, filterText.data(), static_cast<int>(filterText.size()) + 1);

                refreshShortcutList(filterText);
            }
            break;
        }
    }

    return FALSE;
}
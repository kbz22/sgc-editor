#include <windows.h>
#include <commctrl.h>
#include "settings_dialog.h"
#include "program/program.hpp"
#include "settings/shortcut_settings_manager.hpp"
#include "settings/settings.hpp"
#include "locale/shortcut_to_string.hpp"

INT_PTR CALLBACK GeneralSettingsDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_INITDIALOG:
        {
            return TRUE;
        }

        case WM_COMMAND:
        {
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
            auto shortcutSetting = programContext.GetManager<settings::SettingsManager>()->GetSetting<settings::ShortcutsSetting>(settings::ShortcutSettingKey);
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
            auto shortcutSetting = programContext.GetManager<settings::SettingsManager>()->GetSetting<settings::ShortcutsSetting>(settings::ShortcutSettingKey);
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
    auto shortcutSetting = programContext.GetManager<settings::SettingsManager>()->GetSetting<settings::ShortcutsSetting>(settings::ShortcutSettingKey);
    auto &shortcutSettingsManager = shortcutSetting->GetShortcutSettingsManager();

    switch (msg)
    {
        case WM_INITDIALOG:
        {   
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

            LVCOLUMN column{};

            column.mask = LVCF_TEXT | LVCF_WIDTH;

            column.pszText = const_cast<LPWSTR>(L"Action");
            column.cx = c_actionColumnWidth;
            ListView_InsertColumn(listView, 0, &column);

            column.pszText = const_cast<LPWSTR>(L"Shortcut");
            column.cx = c_shortcutColumnWidth;
            ListView_InsertColumn(listView, 1, &column);

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

                if(shortcuts.empty())
                {
                    insertItem(win32_program::Shortcut{}, shortcutSettingsManager);
                }

                for(auto &shortcut : shortcuts)
                {
                    insertItem(shortcut, shortcutSettingsManager);
                }
            }

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
                        auto str = stringLookup.Get(locale::StringId::ShortcutTextSetNewShortcut).value_or(L"");

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

                        SetTimer(hDlg, gc_TimerId, 3500, nullptr);
                        
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
                // auto shortcutManager = programContext.GetManager<win32_program::ShortcutManager>();
                auto editedShortcutPtr = shortcutSettingsManager.GetEditedShortcut();
                auto editedShortcut = *editedShortcutPtr; // copy!!

                // shortcutSettingsManager.CommitEdit(*shortcutManager);

                auto listView = GetDlgItem(hDlg, IDC_SHORTCUT_LIST);

                ListView_SetItemText(
                    listView,
                    editedShortcut.index,
                    1,
                    const_cast<LPWSTR>(editedShortcut.shortcutString.c_str())
                );
            }
            break;
        }
    }

    return FALSE;
}
#include <windows.h>
#include <commctrl.h>
#include "settings_dialog.h"
#include "program/program.hpp"
#include "win32_program/shortcut_manager.hpp"
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

INT_PTR CALLBACK ShortcutsSettingsDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto &programContext = program::GetProgramContext();

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            auto listView = GetDlgItem(hDlg, IDC_SHORTCUT_LIST);

            ListView_SetExtendedListViewStyle(
                listView,
                LVS_EX_FULLROWSELECT |
                LVS_EX_DOUBLEBUFFER |
                LVS_EX_HEADERDRAGDROP
            );

            LVCOLUMN column{};

            column.mask = LVCF_TEXT | LVCF_WIDTH;

            column.pszText = const_cast<LPWSTR>(L"Action");
            column.cx = 200;
            ListView_InsertColumn(listView, 0, &column);

            column.pszText = const_cast<LPWSTR>(L"Shortcut");
            column.cx = 120;
            ListView_InsertColumn(listView, 1, &column);

            auto &stringLookup = programContext.stringLookup;
            auto &shortcutManager = *programContext.shortcutManager;            
            auto allActions = programContext.actionManager->GetActions();

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

                auto shorcuts = shortcutManager.GetShortcutsForAction(action->GetType());

                if(shorcuts.empty())
                {
                    LVITEM item{};
                    item.mask = LVIF_TEXT;
                    item.pszText = const_cast<LPWSTR>(actionName.c_str());
                    item.iItem = ListView_GetItemCount(listView);
                    ListView_InsertItem(listView, &item);

                    ListView_SetItemText(
                        listView,
                        item.iItem,
                        1,
                        const_cast<LPWSTR>(L"")
                    );
                }

                for(auto &shortcut : shorcuts)
                {
                    auto str = locale::ShortcutToString(shortcut, programContext);

                    if(str.empty())
                        continue;

                    LVITEM item{};
                    item.mask = LVIF_TEXT;
                    item.pszText = const_cast<LPWSTR>(actionName.c_str());
                    item.iItem = ListView_GetItemCount(listView);
                    ListView_InsertItem(listView, &item);

                    ListView_SetItemText(
                        listView,
                        item.iItem,
                        1,
                        const_cast<LPWSTR>(str.c_str())
                    );
                }
            }

            return TRUE;
        }

        case WM_COMMAND:
        {
            break;
        }
    }
    return FALSE;
}
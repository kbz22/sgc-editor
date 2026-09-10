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
            constexpr int c_actionColumnWidth = 130;
            constexpr int c_shortcutColumnWidth = 2 * c_actionColumnWidth;

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

                auto insertItem = [&action, &actionName, &listView](const std::wstring &text){
                    LVITEM item{};
                    item.mask = LVIF_TEXT | LVIF_PARAM;
                    item.lParam = static_cast<LPARAM>(action->GetType());
                    item.pszText = const_cast<LPWSTR>(actionName.c_str());
                    item.iItem = ListView_GetItemCount(listView);
                    ListView_InsertItem(listView, &item);

                    ListView_SetItemText(
                        listView,
                        item.iItem,
                        1,
                        const_cast<LPWSTR>(text.c_str())
                    );
                };

                if(shorcuts.empty())
                {
                    insertItem(L"");
                }

                for(auto &shortcut : shorcuts)
                {
                    auto str = locale::ShortcutToString(shortcut, programContext);

                    if(str.empty())
                        continue;

                    insertItem(str);
                }
            }

            return TRUE;
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

                        int row = info->iItem;
                        int column = info->iSubItem;
                        auto listView = GetDlgItem(hDlg, IDC_SHORTCUT_LIST);

                        LVITEM item{};
                        item.mask = LVIF_PARAM;
                        item.iItem = info->iItem;

                        ListView_GetItem(listView, &item);

                        auto &stringLookup = programContext.stringLookup;
                        auto str = stringLookup.Get(locale::StringId::ShortcutTextSetNewShortcut).value_or(L"");

                        ListView_SetItemText(
                            listView,
                            item.iItem,
                            1,
                            const_cast<LPWSTR>(str.c_str())
                        );
                        
                        break;
                    }
                }
            }

            break;
        }
    }
    return FALSE;
}
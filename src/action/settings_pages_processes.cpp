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

LRESULT CALLBACK ShortcutsSettingsListViewProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, [[maybe_unused]]UINT_PTR id, DWORD_PTR data)
{
    switch (msg)
    {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_CHAR:
        case WM_SYSCHAR:
        {
            auto &shortcutManager = *reinterpret_cast<win32_program::ShortcutManager *>(data);

            if(shortcutManager.GetEditedShortcutType().has_value())
                return 0;

            break;
        }
    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
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
            
            SetWindowSubclass(
                listView,
                ShortcutsSettingsListViewProc,
                0,
                reinterpret_cast<DWORD_PTR>(programContext.GetManager<win32_program::ShortcutManager>())
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

        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_LBUTTONDOWN:
        {
            auto shortcutManager = programContext.GetManager<win32_program::ShortcutManager>();
            shortcutManager->ResetEditedShortcut();
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

                        int row = info->iItem;
                        int column = info->iSubItem;
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

                        auto shortcutManager = programContext.GetManager<win32_program::ShortcutManager>();
                        auto shortcut = win32_program::Shortcut{};
                        shortcutManager->SetEditedShortcut(static_cast<action::ActionType>(item.lParam), shortcut);
                        
                        break;
                    }
                }
            }

            break;
        }
    }
    return FALSE;
}
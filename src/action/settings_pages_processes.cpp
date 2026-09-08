#include <windows.h>

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
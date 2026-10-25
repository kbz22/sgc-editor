#include "action/symbol_configure_action.hpp"
#include "program/program.hpp"
#include "sgc_view/symbol_tileset_view.hpp"
#include "symbol_config_dialog.h"
#include <windows.h>
#include <commdlg.h>

bool action::SymbolConfigureAction::m_registered = false;

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
    if(!m_registered) 
    {
        const wchar_t* className = L"SymbolTilesetControl";

        WNDCLASSEX wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = SymbolTilesetProc;
        wc.hInstance = context.GetHInstance();
        wc.lpszClassName = className;        
        RegisterClassEx(&wc);

        m_registered = true;
    }
    
    if(context.GetSection<sections::TilesetSection>() != nullptr) 
    {
        auto hMainWindow = context.GetMainWindowHandle();
        auto hInstance = context.GetHInstance();
        if(hMainWindow != nullptr)
        {
            DialogBox(
                hInstance,            
                MAKEINTRESOURCE(IDD_SYMBOL_CONFIG),
                hMainWindow,
                SymbolConfigureDialogProc
            );
        }
    }
}

INT_PTR CALLBACK SymbolConfigureDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static std::unique_ptr<sgc_view::SymbolTilesetView> tilesetView = nullptr;
    auto &programContext = program::GetProgramContext();

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            using namespace locale;
            using namespace sgc_view;

            auto hwnd = GetDlgItem(hDlg, IDC_SYMBOL_TILESET_VIEW);

            RECT rect;
            GetWindowRect(hwnd, &rect);

            tilesetView = std::make_unique<SymbolTilesetView>(hwnd);
            auto mapDocument = programContext.GetManager<file::FileManager>()->GetActiveDocument();

            tilesetView->SetTileset(mapDocument->GetTilesetAssetId());
            tilesetView->SetScreenSize(rect.right - rect.left, rect.bottom - rect.top);
            tilesetView->SetCursorColor({251,192,27,255});
            tilesetView->Refresh(programContext);
            tilesetView->Render();

            auto &stringLookup = programContext.GetStringLookup();
            auto symbolGroupName = stringLookup.Get(StringId::SymbolDialogSymbolGroupName).value_or(L"GROUP NAME");
            auto addName = stringLookup.Get(StringId::SymbolDialogAddName).value_or(L"ADD");
            auto groupSettingsName = stringLookup.Get(StringId::SymbolDialogGroupSettingsName).value_or(L"GROUP SETTINGS NAME");
            auto widthName = stringLookup.Get(StringId::SymbolDialogWidthName).value_or(L"WIDTH");
            auto heightName = stringLookup.Get(StringId::SymbolDialogHeightName).value_or(L"HEIGHT");
            auto saveName = stringLookup.Get(StringId::DialogSave).value_or(L"SAVE");
            auto cancelName = stringLookup.Get(StringId::DialogCancel).value_or(L"CANCEL");

            SetDlgItemTextW(hDlg, IDOK, saveName.c_str());
            SetDlgItemTextW(hDlg, IDCANCEL, cancelName.c_str());
            SetDlgItemTextW(hDlg, IDC_SYMBOL_GROUP_LABEL, symbolGroupName.c_str());
            SetDlgItemTextW(hDlg, IDC_SYMBOL_ADD_GROUP, addName.c_str());
            SetDlgItemTextW(hDlg, IDC_SYMBOL_GROUP_CONFIG, groupSettingsName.c_str());
            SetDlgItemTextW(hDlg, IDC_SYMBOL_SIZE_X_LABEL, widthName.c_str());
            SetDlgItemTextW(hDlg, IDC_SYMBOL_SIZE_Y_LABEL, heightName.c_str());            

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

                default:
                {
                    return TRUE;
                }
            }            
        }
    }

    return FALSE;
}

LRESULT CALLBACK SymbolTilesetProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_COMMAND:
        {
            auto commandId = LOWORD(wParam);

            switch (commandId)
            {
                case IDOK:
                {
                    EndDialog(hwnd, IDCANCEL);
                    return TRUE;
                }

                case IDCANCEL:
                {
                    EndDialog(hwnd, IDCANCEL);
                    return TRUE;
                }

                default:
                {
                    return TRUE;
                }
            }            
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

#include "action/symbol_configure_action.hpp"
#include "win32_helpers/resize_helpers.hpp"
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

            auto tilesetViewHwnd = GetDlgItem(hDlg, IDC_SYMBOL_TILESET_VIEW);

            RECT rect;
            GetWindowRect(tilesetViewHwnd, &rect);

            tilesetView = std::make_unique<SymbolTilesetView>(tilesetViewHwnd);
            auto mapDocument = programContext.GetManager<file::FileManager>()->GetActiveDocument();

            tilesetView->SetTileset(mapDocument->GetTilesetAssetId());
            tilesetView->SetScreenSize(rect.right - rect.left, rect.bottom - rect.top);
            tilesetView->SetCursorColor({251,192,27,255});
            tilesetView->Refresh(programContext);            

            auto imageSize = tilesetView->GetImageSize();
            win32_helpers::ResizeScrollBarsForImage(
                tilesetViewHwnd,
                static_cast<int>(imageSize.x),
                static_cast<int>(imageSize.y)
            );

            tilesetView->Render();

            SetWindowLongPtrW(
                tilesetViewHwnd,
                GWLP_USERDATA,
                reinterpret_cast<LONG_PTR>(tilesetView.get())
            );

            // localized text
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
        case WM_HSCROLL:
        case WM_VSCROLL:
        {
            const bool isHorizontal = (msg == WM_HSCROLL);
            const int bar = isHorizontal ? SB_HORZ : SB_VERT;

            SCROLLINFO si{};
            si.cbSize = sizeof(si);
            si.fMask = SIF_ALL;

            GetScrollInfo(hwnd, bar, &si);

            int position = si.nPos;

            switch (LOWORD(wParam))
            {
            case SB_LINELEFT:
                position -= 16;
                break;

            case SB_LINERIGHT:
                position += 16;
                break;

            case SB_PAGELEFT:
                position -= static_cast<int>(si.nPage);
                break;

            case SB_PAGERIGHT:
                position += static_cast<int>(si.nPage);
                break;

            case SB_THUMBTRACK:
                position = si.nTrackPos;
                break;

            case SB_THUMBPOSITION:
                position = si.nTrackPos;
                break;

            default:
                return 0;
            }

            const int maxPosition = std::max(
                si.nMin,
                si.nMax - static_cast<int>(si.nPage) + 1
            );

            position = std::clamp(position, si.nMin, maxPosition);

            si.fMask = SIF_POS;
            si.nPos = position;

            SetScrollInfo(hwnd, bar, &si, TRUE);

            auto tilesetView = reinterpret_cast<sgc_view::SymbolTilesetView*>(
                GetWindowLongPtrW(hwnd, GWLP_USERDATA)
            );
            tilesetView->SetCameraPositionSingles(
                isHorizontal  ? static_cast<float>(position) : 0,
                !isHorizontal ? static_cast<float>(position) : 0
            );
            tilesetView->Render();
            
            return 0;
        }
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

#include "sections/package_section.hpp"
#include "program/program.hpp"
#include <commctrl.h>

sections::PackageSection::PackageSection(program::ProgramContext& programContext) :
    Section{L"PackageList", win32_program::ControlId::PackageView, *programContext.mainWindowContext}
{
    RECT rect;
    GetClientRect(programContext.mainWindowContext->hMainWindow, &rect);

    m_packageTreeViewHandle = CreateWindowEx(
        0,
        WC_TREEVIEW,
        L"",
        WS_VISIBLE | WS_CHILD |
        TVS_HASLINES |
        TVS_LINESATROOT |
        TVS_HASBUTTONS |
        TVS_SHOWSELALWAYS,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
        GetHwnd(),
        nullptr,
        programContext.mainWindowContext->hInstance,
        nullptr
    );

    TreeView_SetImageList(
        m_packageTreeViewHandle,
        programContext.packageViewFileIcons,
        TVSIL_NORMAL
    );
}

sections::PackageSection::~PackageSection()
{
    if (m_packageTreeViewHandle) {
        DestroyWindow(m_packageTreeViewHandle);
        m_packageTreeViewHandle = nullptr;
    }
}

LRESULT sections::PackageSection::HandleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg) {
        case WM_NOTIFY: {
            LPNMHDR lpnmhdr = reinterpret_cast<LPNMHDR>(lparam);
            if (lpnmhdr->hwndFrom == m_packageTreeViewHandle) {
                // Handle notifications from the tree view control here
            }
            break;
        }
        default:
            break;
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

void sections::PackageSection::Update()
{
    RECT rect;
    GetClientRect(m_packageTreeViewHandle, &rect);

    SetWindowPos(m_packageTreeViewHandle, nullptr, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);

    Redraw();
    return;
}

void sections::PackageSection::HandleSectionResize()
{
    return;
}

void sections::PackageSection::Refresh(program::ProgramContext& programContext)
{
    auto addItem = [this](const std::wstring& text, LPARAM lParam, int imageIndex, HTREEITEM hParent = TVI_ROOT) {
        TVINSERTSTRUCT insert{};
        insert.hParent = hParent;
        insert.hInsertAfter = TVI_LAST;

        insert.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
        insert.item.pszText = const_cast<wchar_t*>(text.c_str());
        insert.item.lParam = lParam;

        insert.item.iImage = imageIndex;
        insert.item.iSelectedImage = imageIndex;

        return TreeView_InsertItem(m_packageTreeViewHandle, &insert);
    };

    TreeView_DeleteAllItems(m_packageTreeViewHandle);

    auto allFiles = programContext.fileManager->GetOpenFiles();

    for(auto file : allFiles) {

        auto root = TVI_ROOT;
        auto imageIndex = static_cast<int>(file->GetFileType());
        if(file->IsContainer()) {
            root = addItem(file->GetFileName(), reinterpret_cast<LPARAM>(file), imageIndex, TVI_ROOT);
        }

        for(auto maps : file->GetMapDocuments()) {
            addItem(maps->GetName(), reinterpret_cast<LPARAM>(file), imageIndex, root);
        }
        for(auto tilesets : file->GetTilesetDocuments()) {
            addItem(tilesets->GetName(), reinterpret_cast<LPARAM>(file), imageIndex, root);
        }
    }
    
    return;
}

void sections::PackageSection::RegisterFileActionCallback(sections::FileAction action, std::function<void(const std::wstring&, size_t)> callback)
{
    m_fileActionCallbacks[action] = callback;
}
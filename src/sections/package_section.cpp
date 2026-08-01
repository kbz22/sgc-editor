#include "sections/package_section.hpp"
#include "program/program.hpp"
#include "file/itreeviewlistable.hpp"

sections::PackageSection::PackageSection(program::ProgramContext& programContext) :
    Section{L"PackageList", win32_program::ControlId::PackageView, *programContext.mainWindowContext}
{
    RECT rect;
    GetClientRect(GetHwnd(), &rect);

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

    SetupSubclass(GetHwnd(), this);
    
}

sections::PackageSection::~PackageSection()
{
    if (m_packageTreeViewHandle) {
        DestroyWindow(m_packageTreeViewHandle);
        m_packageTreeViewHandle = nullptr;
    }
}

void sections::PackageSection::TreeViewNotifyHandler(NMTREEVIEW* nm, program::ProgramContext& programContext)
{
    auto code = nm->hdr.code;

    switch(code){

        case TVN_SELCHANGED:
        {
            using namespace file;            

            auto listItem = reinterpret_cast<TreeListItem*>(nm->itemNew.lParam);

            if(listItem != nullptr && listItem->listable != nullptr && listItem->file != nullptr) 
            {
                auto callbackIt = m_fileActionCallbacks.find(FileAction::ItemSelected);
                if(callbackIt != m_fileActionCallbacks.end()) 
                {
                    callbackIt->second(listItem->file, listItem->inFileIndex);
                }
            }

            break;
        }
        

        case NM_DBLCLK:
        {

            break;
        }

        default:
            break;

    }
}

LRESULT sections::PackageSection::HandleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    auto &programContext = program::GetProgramContext();

    switch (msg) {

        case WM_NOTIFY: 
        {
            auto *info = reinterpret_cast<NMTREEVIEW*>(lparam);
            if (info->hdr.hwndFrom == m_packageTreeViewHandle)
            {
                TreeViewNotifyHandler(info, programContext);
            }
        }

        default:
            break;

    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

void sections::PackageSection::Update()
{
    Redraw();
    return;
}

void sections::PackageSection::HandleSectionResize()
{
    RECT rect;
    GetClientRect(GetHwnd(), &rect);

    MoveWindow(
        m_packageTreeViewHandle,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
        TRUE
    );

    InvalidateRect(m_packageTreeViewHandle, nullptr, TRUE);

    return;
}

void sections::PackageSection::UpdateTreeItem(TreeListItem &tli)
{   
    auto name = tli.file->IsDirty() ? L" *" + tli.listable->GetName() : tli.listable->GetName();

    TVITEM item{};
    item.mask = TVIF_TEXT;
    item.hItem = tli.treeItem;
    item.pszText = const_cast<wchar_t*>(name.c_str());

    TreeView_SetItem(m_packageTreeViewHandle, &item);
}

void sections::PackageSection::UpdateSelectedTreeViewItem(program::ProgramContext& programContext)
{
    auto selectedDocument = programContext.fileManager->GetSelectedDocument();

    if(selectedDocument == nullptr) {
        return;
    }

    for(auto &tli : m_treeListItems) {
        if(tli.listable == selectedDocument) {
            TreeView_SelectItem(m_packageTreeViewHandle, tli.treeItem);
            UpdateTreeItem(tli);
            break;
        }
    }
}

void sections::PackageSection::Refresh(program::ProgramContext& programContext)
{
    auto addItem = [this](file::IFile *file, file::ITreeViewListable *listable, HTREEITEM hParent = TVI_ROOT)
    {
        m_treeListItems.push_back(TreeListItem{
            listable,
            file,
            nullptr
        });

        auto &returnItem = m_treeListItems.back();
        auto imageIndex = static_cast<int>(file->GetFileType());

        TVINSERTSTRUCT insert{};
        insert.hParent = hParent;
        insert.hInsertAfter = TVI_LAST;

        insert.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
        insert.item.pszText = const_cast<wchar_t*>(listable->GetName().c_str());
        insert.item.lParam = reinterpret_cast<LPARAM>(&m_treeListItems.back());

        insert.item.iImage = imageIndex;
        insert.item.iSelectedImage = imageIndex;

        returnItem.treeItem = TreeView_InsertItem(m_packageTreeViewHandle, &insert);

        UpdateTreeItem(returnItem);

        return returnItem;
    };

    TreeView_DeleteAllItems(m_packageTreeViewHandle);
    m_treeListItems.clear();

    auto allFiles = programContext.fileManager->GetOpenFiles();

    for(auto file : allFiles) {

        auto root = TVI_ROOT;        

        if(file->IsContainer()) {
            auto tli = addItem(file, reinterpret_cast<file::ITreeViewListable*>(file), TVI_ROOT);
            root = tli.treeItem;
        }

        for(auto mapDoc : file->GetMapDocuments()) 
        {
            auto listable = reinterpret_cast<file::ITreeViewListable*>(mapDoc);
            addItem(file, listable, root);
        }

        for(auto tilesetDoc : file->GetTilesetDocuments()) 
        {
            auto listable = reinterpret_cast<file::ITreeViewListable*>(tilesetDoc);
            addItem(file, listable, root);
        }

    }    
    
    return;
}

void sections::PackageSection::RegisterFileActionCallback(sections::FileAction action, std::function<void(file::IFile*, size_t)> callback)
{
    m_fileActionCallbacks[action] = callback;
    return;
}
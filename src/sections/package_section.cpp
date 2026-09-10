#include "sections/package_section.hpp"
#include "program/program.hpp"
#include "file/itreeviewlistable.hpp"

sections::PackageSection::PackageSection(program::ProgramContext& programContext) :
    Section{L"PackageList", win32_program::ControlId::PackageView, programContext.GetMainWindowHandle(), programContext.GetHInstance()}
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
        programContext.GetHInstance(),
        nullptr
    );

    TreeView_SetImageList(
        m_packageTreeViewHandle,
        programContext.GetImageList(program::ImageListType::ListView),
        TVSIL_NORMAL
    );

    SetupSubclass(GetHwnd(), this);
    SetupSubclass(m_packageTreeViewHandle, this);
}

sections::PackageSection::~PackageSection()
{
    if (m_packageTreeViewHandle) {
        DestroyWindow(m_packageTreeViewHandle);
        m_packageTreeViewHandle = nullptr;
    }
}

void sections::PackageSection::TreeViewNotifyHandler(NMTREEVIEW* nm, [[maybe_unused]] program::ProgramContext& programContext)
{
    auto code = nm->hdr.code;

    auto getItemAndCallback = [this](FileAction action) {
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(GetHwnd(), &pt);

        TVHITTESTINFO hit{};
        hit.pt = pt;

        HTREEITEM hItem = TreeView_HitTest(m_packageTreeViewHandle, &hit);

        if (hItem != nullptr)
        {
            TVITEM item{};
            item.mask = TVIF_PARAM;
            item.hItem = hItem;

            if (TreeView_GetItem(m_packageTreeViewHandle, &item))
            {
                auto listItem = reinterpret_cast<TreeListItem*>(item.lParam);

                if (listItem != nullptr &&
                    listItem->listable != nullptr &&
                    listItem->file != nullptr)
                {
                    auto callbackIt = m_fileActionCallbacks.find(action);
                    if (callbackIt != m_fileActionCallbacks.end())
                    {
                        callbackIt->second(listItem->file, listItem->inFileIndex);
                    }
                }
            }
        }
    };

    switch(code){

        case TVN_SELCHANGED:
        {
            getItemAndCallback(FileAction::ItemSelected);
            break;
        }        

        case NM_DBLCLK:
        {
            getItemAndCallback(FileAction::ItemDoubleClicked);
            break;
        }

        case TVN_ITEMEXPANDED:
        {
            auto *item = reinterpret_cast<TreeListItem*>(nm->itemNew.lParam);
            if (item != nullptr)
            {
                item->collapsed = (nm->action == TVE_COLLAPSE);
            }
            // UpdateTreeItem(*item);
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
            break;
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
    auto name = tli.listable->IsDirty() ? L" *" + tli.listable->GetName() : tli.listable->GetName();
    auto state = tli.treeItem == m_activeTreeItem ? TVIS_BOLD : 0;

    TVITEM item{};
    item.mask = TVIF_TEXT | TVIF_STATE;
    item.hItem = tli.treeItem;
    item.pszText = const_cast<wchar_t*>(name.c_str());
    item.stateMask = TVIS_BOLD;
    item.state = state;

    TreeView_SetItem(m_packageTreeViewHandle, &item);
    TreeView_Expand(m_packageTreeViewHandle, tli.treeItem, tli.collapsed ? TVE_COLLAPSE : TVE_EXPAND);
}

void sections::PackageSection::SetTreeItemActive(TreeListItem &tli)
{
    m_activeTreeItem = tli.treeItem;
}

void sections::PackageSection::UpdateTreeViewItems(program::ProgramContext& programContext)
{
    auto fileManager = programContext.GetManager<file::FileManager>();
    auto activeDocument = fileManager->GetActiveDocument();

    m_activeTreeItem = nullptr;
    
    for(auto &tli : m_treeListItems) {        
        if(tli->listable == activeDocument) 
        {
            SetTreeItemActive(*tli);
        }

        UpdateTreeItem(*tli);
    }
}

void sections::PackageSection::Refresh(program::ProgramContext& programContext)
{
    auto addItem = [this](file::IFile *file, file::ITreeViewListable *listable, HTREEITEM hParent = TVI_ROOT, size_t index = 0, bool collapsed = true) -> TreeListItem*
    {
        m_treeListItems.push_back(std::make_unique<TreeListItem>(
            TreeListItem{
                listable,
                file,
                nullptr,
                index,
                collapsed
            }
        ));

        auto &returnItem = m_treeListItems.back();
        auto imageIndex = static_cast<int>(listable->GetListableType());

        TVINSERTSTRUCT insert{};
        insert.hParent = hParent;
        insert.hInsertAfter = TVI_LAST;

        insert.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
        insert.item.pszText = const_cast<wchar_t*>(listable->GetName().c_str());
        insert.item.lParam = reinterpret_cast<LPARAM>(m_treeListItems.back().get());

        insert.item.iImage = imageIndex;
        insert.item.iSelectedImage = imageIndex;

        returnItem->treeItem = TreeView_InsertItem(m_packageTreeViewHandle, &insert);

        UpdateTreeItem(*returnItem);

        return returnItem.get();
    };

    TreeView_DeleteAllItems(m_packageTreeViewHandle);
    // m_treeListItems.clear();
    auto oldTreeListItems = std::move(m_treeListItems);
    m_treeListItems.clear();

    auto fileManager = programContext.GetManager<file::FileManager>();
    auto allFiles = fileManager->GetOpenFiles();    

    for(auto file : allFiles) 
    {        
        size_t index = 0;
        auto root = TVI_ROOT;
        auto docs = file->GetDocuments();

        for(auto &doc : docs)
        {
            if(doc->IsContainer())
            {
                bool collapsed = false;

                for(auto &oldFile : oldTreeListItems) 
                {
                    if(oldFile->file == file) 
                    {
                        collapsed = oldFile->collapsed;
                        oldTreeListItems.erase(std::remove(oldTreeListItems.begin(), oldTreeListItems.end(), oldFile), oldTreeListItems.end());
                        break;
                    }
                }

                auto tli = addItem(file, dynamic_cast<file::ITreeViewListable*>(file), TVI_ROOT, index++, collapsed);
                root = tli->treeItem;
            }
            else
            {
                addItem(file, dynamic_cast<file::ITreeViewListable*>(doc), root, index++);
            }
        }

    }

    UpdateTreeViewItems(programContext);
    
    return;
}

void sections::PackageSection::RegisterFileActionCallback(sections::FileAction action, std::function<void(file::IFile*, size_t)> callback)
{
    m_fileActionCallbacks[action] = callback;
    return;
}
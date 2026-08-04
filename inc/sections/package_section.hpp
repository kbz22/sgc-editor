#pragma once

#include "sections/section.hpp"
#include <windows.h>
#include <commctrl.h>
#include <unordered_map>
#include <functional>
#include <memory>
#include "file/ifile.hpp"

namespace program {
    struct ProgramContext;
}

namespace sections {

    enum class FileAction {
        ItemSelected,
        ItemDoubleClicked,
        FileSaved,
        FileClosed
    };

    struct TreeListItem {        
        file::ITreeViewListable* listable;
        file::IFile* file;
        HTREEITEM treeItem;
        size_t inFileIndex = 0;
    };

    class PackageSection : public Section
    {
        private:
            HWND m_packageTreeViewHandle = HWND();
            HTREEITEM m_activeTreeItem = nullptr;
            std::unordered_map<FileAction, std::function<void(file::IFile*, size_t)>> m_fileActionCallbacks;
            std::vector<std::shared_ptr<TreeListItem>> m_treeListItems;

            void UpdateTreeItem(TreeListItem &tli);
            void SetTreeItemActive(TreeListItem &tli);
            void TreeViewNotifyHandler(NMTREEVIEW* nm, program::ProgramContext& programContext);
            
        protected:
            LRESULT HandleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;

        public:
            PackageSection(program::ProgramContext& programContext);
            ~PackageSection();

            void Update() override;
            void HandleSectionResize() override;
            void Refresh(program::ProgramContext& programContext) override;

            void UpdateTreeViewItems(program::ProgramContext& programContext);

            void RegisterFileActionCallback(FileAction action, std::function<void(file::IFile*, size_t)> callback);
    };

}
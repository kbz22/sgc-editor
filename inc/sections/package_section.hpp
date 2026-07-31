#pragma once

#include "sections/section.hpp"
#include <windows.h>
#include <functional>
#include <unordered_map>

namespace program {
    struct ProgramContext;
}

namespace sections {

    enum class FileAction {
        FileSelected,
        FileDoubleClicked,
        FileSaved,
        FileClosed
    };

    class PackageSection : public Section
    {
        private:
            HWND m_packageTreeViewHandle = HWND();
            std::unordered_map<FileAction, std::function<void(const std::wstring&, size_t)>> m_fileActionCallbacks;
            
        protected:
            LRESULT HandleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;

        public:
            PackageSection(program::ProgramContext& programContext);
            ~PackageSection();

            void Update() override;
            void HandleSectionResize() override;
            void Refresh(program::ProgramContext& programContext) override;

            void RegisterFileActionCallback(FileAction action, std::function<void(const std::wstring&, size_t)> callback);
    };

}
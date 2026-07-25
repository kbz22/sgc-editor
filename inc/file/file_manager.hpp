#pragma once

#include "file/ifile.hpp"
#include "file/map_document.hpp"

namespace file {

    struct SelectedDocument
    {
        IFile* file;
        size_t index;
    };

    class FileManager
    {
        private:
            std::vector<std::unique_ptr<IFile>> m_openFiles{};
            SelectedDocument m_selectedDocument{nullptr, 0};

        public:
            FileManager();
            ~FileManager();

            void NewFile(std::unique_ptr<IFile> file);
            void OpenFile(std::unique_ptr<IFile> file);
            void SaveFile(size_t index);
            void CloseFile(size_t index);

            void SelectDocument(size_t index);
            SelectedDocument GetSelectedDocument() const;
    };

}
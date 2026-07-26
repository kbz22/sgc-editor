#pragma once

#include "file/ifile.hpp"
#include "file/map_document.hpp"
#include <vector>
#include <memory>
#include <filesystem>

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
            FileManager() = default;
            ~FileManager() = default;

            void NewMapFile(std::filesystem::path filePath, size_t tileWidth, size_t tileHeight);
            void NewTilesetFile(std::filesystem::path filePath, size_t tileWidth, size_t tileHeight);
            void OpenFile(std::filesystem::path filePath);
            void SaveFile(size_t index);
            void CloseFile(size_t index);

            void SelectDocument(size_t index);
            MapDocument* GetSelectedDocument() const;
            size_t GetSelectedFileIndex() const;
    };

}
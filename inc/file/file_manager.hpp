#pragma once

#include "file/ifile.hpp"
#include "file/map_document.hpp"
#include "file/asset_manager.hpp"
#include <vector>
#include <memory>
#include <filesystem>
#include <functional>

namespace file {

    struct DocumentLocation
    {
        IFile* file;
        size_t index;
    };

    class FileManager
    {
        private:
            std::vector<std::unique_ptr<IFile>> m_openFiles{};
            DocumentLocation m_selectedDocument{nullptr, 0};
            DocumentLocation m_activeDocument{nullptr, 0};
            std::function<void(DocumentLocation)> m_onActiveDocumentChangedCallback{nullptr};

        public:
            FileManager() = default;
            ~FileManager() = default;

            void NewMapFile(std::wstring name, sgc::data::AssetId tilesetId);
            void NewTilesetFile(std::wstring name, std::filesystem::path filePath, size_t tileWidth, size_t tileHeight, AssetManager &assetManager);
            void OpenFile(std::filesystem::path filePath);
            void SaveFile(size_t index);
            void CloseFile(size_t index);

            void SelectDocument(const DocumentLocation &document);
            void SetActiveDocument(const DocumentLocation &document);
            MapDocument* GetActiveDocument() const;
            IFile* GetSelectedFile() const;
            size_t GetSelectedFileIndex() const;
            size_t GetFileIndex(IFile* file) const;
            DocumentLocation GetSelectedDocumentLocation() const;
            DocumentLocation GetActiveDocumentLocation() const;

            std::vector<IFile*> GetOpenFiles() const;
            std::vector<TilesetDocument*> GetAllTilesetDocuments() const;

            void RegisterOnActiveDocumentChangedCallback(std::function<void(DocumentLocation)> callback);
    };

}
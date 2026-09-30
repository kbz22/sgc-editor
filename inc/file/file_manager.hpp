#pragma once

#include "file/ifile.hpp"
#include "file/map_document.hpp"
#include "file/asset_manager.hpp"
#include "file/package_file.hpp"
#include "file/tileset_document.hpp"
#include "program/session.hpp"
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
            std::function<void(IFile*)> m_onFileUpdatedCallback{nullptr};
            std::function<void(MapDocument*, bool)> m_onMapDirtyCallback{nullptr};
            program::Session m_sessionJson{};

            void UpdateSessionFile();

        public:
            FileManager() = default;
            ~FileManager() = default;

            void NewMapFile(std::wstring name, sgc::data::AssetId tilesetId);
            MapDocument NewMapDocument(std::wstring name, sgc::data::AssetId tilesetId);
            void NewTilesetFile(std::wstring name, std::filesystem::path filePath, size_t tileWidth, size_t tileHeight, AssetManager &assetManager);
            TilesetDocument NewTilesetDocument(std::wstring name, std::filesystem::path filePath, size_t tileWidth, size_t tileHeight);
            void NewPackageFile(std::wstring name, std::vector<MapDocument*> mapDocuments, std::vector<TilesetDocument*> tilesetDocuments, AssetManager &assetManager);            
            void OpenFile(std::filesystem::path filePath, AssetManager *assetManager);
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
            std::vector<PackageFile*> GetAllPackages() const;

            void RegisterOnActiveDocumentChangedCallback(std::function<void(DocumentLocation)> callback);
            void RegisterOnFileUpdatedCallback(std::function<void(IFile*)> callback);
            void RegisterOnMapDirtyCallback(std::function<void(MapDocument*, bool)> callback);

            void RestoreLastSession(file::AssetManager *assetManager);
    };

}
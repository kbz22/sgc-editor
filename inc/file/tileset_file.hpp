#pragma once

#include "file/ifile.hpp"
#include "file/tileset_document.hpp"
#include "file/asset_manager.hpp"
#include <filesystem>
#include <memory>

namespace file {

    class TilesetFile : public IFile
    {
        private:
            std::filesystem::path m_filePath{};
            std::unique_ptr<TilesetDocument> m_tilesetDocument{nullptr};
            AssetManager &m_assetManager;
            bool m_savedOrLoaded{false};

        public:
            TilesetFile(AssetManager &assetManager);
            virtual ~TilesetFile() = default;

            void Open() override;
            void Save() override;
            void Close() override;

            std::wstring GetExtension() const override;
            std::wstring GetFileName() const override;
            std::filesystem::path GetFilePath() const override;

            bool IsDirty() const override;
            bool IsContainer() const override;
            bool IsActivable() const override;

            FileType GetFileType() const override;

            void SetFilePath(const std::filesystem::path& path) override;

            std::vector<MapDocument*> GetMapDocuments() override;
            std::optional<MapDocument*> GetMapDocument(size_t index) override;
            std::vector<TilesetDocument*> GetTilesetDocuments() override;
            std::optional<TilesetDocument*> GetTilesetDocument(size_t index) override;
            std::vector<IDocument*> GetDocuments() override;
            std::optional<IDocument*> GetDocument(size_t index) override;

            friend class FileManager;
    };

}
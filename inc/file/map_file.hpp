#pragma once

#include "file/ifile.hpp"
#include "file/map_document.hpp"
#include "file/tileset_document.hpp"

#include <memory>
#include <filesystem>

namespace file {

    class FileManager;

    class MapFile : public IFile
    {
        private:
            std::filesystem::path m_filePath{};
            std::unique_ptr<MapDocument> m_document = nullptr;

        public:
            MapFile() = default;
            virtual ~MapFile() = default;

            void Open() override;
            void Save() override;
            void Close() override;

            std::wstring GetExtension() const override;
            std::wstring GetFileName() const override;
            std::filesystem::path GetFilePath() const override;

            bool IsDirty() const override;

            void SetFilePath(const std::filesystem::path& path) override;

            std::vector<MapDocument*> GetMapDocuments() override;
            std::optional<MapDocument*> GetMapDocument(size_t index) override;
            std::vector<TilesetDocument*> GetTilesetDocuments() override;
            std::optional<TilesetDocument*> GetTilesetDocument(size_t index) override;

            friend class FileManager;
    };

}
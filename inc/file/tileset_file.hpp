#pragma once

#include "file/ifile.hpp"
#include "file/tileset_document.hpp"
#include <filesystem>
#include <memory>

namespace file {

    class TilesetFile : public IFile
    {
        private:
            std::filesystem::path m_filePath{};
            std::unique_ptr<TilesetDocument> m_tilesetDocument{nullptr};
            bool m_savedOrLoaded{false};

        public:
            TilesetFile(sgc::data::AssetId tilesetId);
            virtual ~TilesetFile() = default;

            void Open() override;
            void Save() override;
            void Close() override;

            std::wstring GetExtension() const override;            
            std::wstring GetFileName() const override;
            std::filesystem::path GetFilePath() const override;

            bool IsDirty() const override;

            void SetFilePath(const std::filesystem::path& path) override;

            std::span<MapDocument*> GetMapDocuments() override;
            std::span<TilesetDocument*> GetTilesetDocuments() override;
    };

}
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

        public:
            TilesetFile(sgc::data::AssetId tilesetId);
            virtual ~TilesetFile() = default;

            void Open(std::filesystem::path filePath) override;
            void Save(std::filesystem::path filePath) override;
            void Close() override;

            std::wstring GetExtension() const override;
            std::wstring GetFileName() const override;

            std::span<MapDocument*> GetMapDocuments() override;
            std::span<TilesetDocument*> GetTilesetDocuments() override;
    };

}
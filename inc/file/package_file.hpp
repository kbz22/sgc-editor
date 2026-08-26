#pragma once

#include "file/ifile.hpp"
#include "file/itreeviewlistable.hpp"

namespace file {

    const std::wstring_view PackageFileExtension = L".sgcp";

    class PackageFile : public IFile, public ITreeViewListable
    {
        private:
            std::filesystem::path m_filePath{};
            std::wstring m_name{};
            std::vector<std::unique_ptr<MapDocument>> m_mapDocuments{};
            std::vector<std::unique_ptr<TilesetDocument>> m_tilesetDocuments{};

        public:
            PackageFile(std::wstring name);
            ~PackageFile() = default;

            void AddMapDocument(std::unique_ptr<MapDocument> mapDocument);
            void AddTilesetDocument(std::unique_ptr<TilesetDocument> tilesetDocument);

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
            ListableType GetListableType() const override;

            void SetFilePath(const std::filesystem::path& path) override;

            std::vector<MapDocument*> GetMapDocuments() override;
            std::optional<MapDocument*> GetMapDocument(size_t index) override;
            std::vector<TilesetDocument*> GetTilesetDocuments() override;
            std::optional<TilesetDocument*> GetTilesetDocument(size_t index) override;

            const std::wstring& GetName() const override;
    };

}
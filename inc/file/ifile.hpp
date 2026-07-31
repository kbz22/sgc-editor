#pragma once

#include "file/map_document.hpp"
#include "file/tileset_document.hpp"
#include <string>
#include <span>
#include <filesystem>
#include <optional>

namespace file {

    enum class FileType {
        Map = 0,
        Tileset = 1,
        Package = 2
    };

    class IFile
    {
        public:
            virtual ~IFile() = default;
            
            virtual void Open() = 0;
            virtual void Save() = 0;
            virtual void Close() = 0;

            virtual std::wstring GetExtension() const = 0;
            virtual std::wstring GetFileName() const = 0;
            virtual std::filesystem::path GetFilePath() const = 0;

            virtual bool IsDirty() const = 0;
            virtual bool IsContainer() const = 0;

            virtual FileType GetFileType() const = 0;

            virtual void SetFilePath(const std::filesystem::path& path) = 0;

            virtual std::vector<MapDocument*> GetMapDocuments() = 0;
            virtual std::optional<MapDocument*> GetMapDocument(size_t index) = 0;
            virtual std::vector<TilesetDocument*> GetTilesetDocuments() = 0;
            virtual std::optional<TilesetDocument*> GetTilesetDocument(size_t index) = 0;
    };

}
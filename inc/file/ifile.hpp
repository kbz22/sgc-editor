#pragma once

#include "file/map_document.hpp"
#include "file/tileset_document.hpp"
#include <string>
#include <span>
#include <filesystem>

namespace file {

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

            virtual void SetFilePath(const std::filesystem::path& path) = 0;            

            virtual std::span<MapDocument*> GetMapDocuments() = 0;
            virtual std::span<TilesetDocument*> GetTilesetDocuments() = 0;
    };

}
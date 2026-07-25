#pragma once

#include "file/map_document.hpp"
#include <string>
#include <span>
#include <filesystem>

namespace file {

    class IFile
    {
        public:
            virtual ~IFile() = default;
            
            virtual void Open(std::filesystem::path filePath) = 0;
            virtual void Save(std::filesystem::path filePath) = 0;
            virtual void Close() = 0;

            virtual std::wstring GetExtension() const = 0;
            virtual std::wstring GetFileName() const = 0;

            virtual std::span<MapDocument*> GetMapDocuments() = 0;
    };

}
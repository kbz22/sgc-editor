#pragma once

#include "file/ifile.hpp"
#include "defaults.hpp"
#include "file/map_document.hpp"

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

            void Open(std::filesystem::path filePath) override;
            void Save(std::filesystem::path filePath) override;
            void Close() override;

            std::wstring GetExtension() const override;
            std::wstring GetFileName() const override;
            std::span<MapDocument*> GetMapDocuments() override;

            friend class FileManager;
    };

}
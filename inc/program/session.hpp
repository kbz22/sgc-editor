#pragma once

#include <nlohmann/json.hpp>
#include <vector>
#include <filesystem>
#include "file/ifile.hpp"
#include "file/document_location.hpp"

namespace program 
{    
    class Session
    {
        private:
            nlohmann::json m_json{};            
            std::wstring m_sessionFileName = L"session.json";
            const std::string m_openedFilesString = "open-files";
            const std::string m_activeDocumentFileString = "active-document-file";
            const std::string m_activeDocumentIndexString = "active-document-index";

        public:
            void Load();
            void Save();

            void RebuildFileList(std::vector<std::unique_ptr<file::IFile>> const &files);
            std::vector<std::filesystem::path> GetSessionPaths();

            void SetActiveDocument(file::DocumentLocation &location);
            std::filesystem::path GetActiveDocumentPath();
            size_t GetActiveDocumentIndex();

    };
}
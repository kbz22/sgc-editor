#pragma once

#include <nlohmann/json.hpp>
#include <vector>
#include <filesystem>
#include "file/ifile.hpp"

namespace program 
{    
    class Session
    {
        private:
            nlohmann::json m_json{};            
            std::wstring m_sessionFileName = L"session.json";
            const std::string m_openedFilesString = "open-files";

        public:
            void Load();
            void Save();

            void RebuildFileList(std::vector<std::unique_ptr<file::IFile>> const &files);
            std::vector<std::filesystem::path> GetSessionPaths();

    };
}
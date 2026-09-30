#include "program/session.hpp"
#include "win32_helpers/file_helpers.hpp"
#include <fstream>

void program::Session::Load()
{
    std::filesystem::path path = win32_helpers::GetPreferencesDirectory();
    path /= m_sessionFileName;
    std::ifstream file(path);

    if (!file)
    {
        throw std::runtime_error("Failed to open preferences file for reading");
    }
    
    file >> m_json;
}

void program::Session::Save()
{
    std::filesystem::path path = win32_helpers::GetPreferencesDirectory();
    path /= m_sessionFileName;
    std::ofstream file(path);

    if (!file)
    {
        throw std::runtime_error("Failed to open preferences file for writing");
    }

    file << m_json.dump(4);
}

void program::Session::RebuildFileList(std::vector<std::unique_ptr<file::IFile>> const &files)
{
    std::vector<std::filesystem::path> paths;
    for(auto &file : files)
    {
        paths.push_back(file->GetFilePath());
    }

    m_json[m_openedFilesString] = paths;
}

std::vector<std::filesystem::path> program::Session::GetSessionPaths()
{
    auto paths = m_json[m_openedFilesString].get<std::vector<std::filesystem::path>>();
    return paths;    
}
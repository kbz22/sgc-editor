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

void program::Session::SetActiveDocument(file::DocumentLocation &location)
{    
    m_json[m_activeDocumentFileString] = location.file->GetFilePath();
    m_json[m_activeDocumentIndexString] = location.index;
}

size_t program::Session::GetActiveDocumentIndex()
{
    return m_json[m_activeDocumentIndexString].get<size_t>();
}

std::filesystem::path program::Session::GetActiveDocumentPath()
{
    return m_json[m_activeDocumentFileString].get<std::filesystem::path>();
}

void program::Session::SetWindowPositionAndSize(int x, int y, int width, int height)
{
    m_json[m_windowPositionXString] = x;
    m_json[m_windowPositionYString] = y;
    m_json[m_windowWidthString] = width;
    m_json[m_windowHeightString] = height;

    return;
}

bool program::Session::GetWindowPositionAndSize(int &x, int &y, int &width, int &height)
{
    bool containsAll = (
        m_json.contains(m_windowPositionXString) &&
        m_json.contains(m_windowPositionYString) &&
        m_json.contains(m_windowWidthString) &&
        m_json.contains(m_windowHeightString)
    );
        
    if(containsAll)
    {
        x = m_json[m_windowPositionXString];
        y = m_json[m_windowPositionYString];
        width = m_json[m_windowWidthString];
        height = m_json[m_windowHeightString];   
        return true;
    }

    return false;
}

void program::Session::SetCameraPosition(float x, float y)
{
    m_json[m_cameraXString] = x;
    m_json[m_cameraYString] = y;

    return;
}

bool program::Session::GetCameraPosition(float &x, float &y)
{
    if(m_json.contains(m_cameraXString) && m_json.contains(m_cameraYString))
    {
        x = m_json[m_cameraXString];
        y = m_json[m_cameraYString];
        return true;
    }

    return false;
}

void program::Session::SetZoom(float zoom)
{
    m_json[m_zoomString] = zoom;

    return;
}

bool program::Session::GetZoom(float &zoom)
{    
    if(m_json.contains(m_zoomString))
    {
        zoom = m_json[m_zoomString];
        return true;
    }

    return false;
}
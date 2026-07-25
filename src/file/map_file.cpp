#include "file/map_file.hpp"

void file::MapFile::Open(std::filesystem::path filePath){
    return;
}

void file::MapFile::Save(std::filesystem::path filePath){
    return;
}

void file::MapFile::Close(){
    return;
}

std::wstring file::MapFile::GetExtension() const
{
    auto extension = m_filePath.extension().wstring();
    return extension.empty() ? L"" : extension.substr(1); // Remove the leading dot
}

std::wstring file::MapFile::GetFileName() const
{
    auto fileName = m_filePath.filename().wstring();
    return fileName;
}

std::span<file::MapDocument*> file::MapFile::GetMapDocuments()
{
    auto ptr = m_document.get();
    if (m_document) {
        return std::span<MapDocument*>(&ptr, 1);
    } else {
        return std::span<MapDocument*>();
    }
}
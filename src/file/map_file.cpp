#include "file/map_file.hpp"
#include "defaults.hpp"

void file::MapFile::Open(){
    return;
}

void file::MapFile::Save(){
    return;
}

void file::MapFile::Close(){
    return;
}

std::wstring file::MapFile::GetExtension() const
{
    auto extension = m_filePath.extension().wstring();

    return (
        extension.empty() ?
        defaults::MapFileExtension.data() :
        extension
    );
}

std::wstring file::MapFile::GetFileName() const
{
    auto fileName = m_filePath.filename().wstring();
    return fileName;
}

std::filesystem::path file::MapFile::GetFilePath() const
{
    return m_filePath;
}

void file::MapFile::SetFilePath(const std::filesystem::path& path)
{
    m_filePath = path;
}

bool file::MapFile::IsDirty() const
{
    if (m_document) {
        return m_document->IsDirty();
    }
    return false;
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

std::span<file::TilesetDocument*> file::MapFile::GetTilesetDocuments()
{
    return std::span<TilesetDocument*>();
}
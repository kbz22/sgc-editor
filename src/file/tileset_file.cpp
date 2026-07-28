#include "file/tileset_file.hpp"
#include "defaults.hpp"

void file::TilesetFile::Open(){
    return;
}

void file::TilesetFile::Save(){

    if(m_filePath.empty()) {
        return;
    }

    m_savedOrLoaded = true;

    return;
}

void file::TilesetFile::Close(){
    return;
}

std::wstring file::TilesetFile::GetExtension() const
{
    auto extension = m_filePath.extension().wstring();

    return (
        extension.empty() ?
        defaults::TilesetFileExtension.data() :
        extension
    );
}

std::wstring file::TilesetFile::GetFileName() const
{
    auto fileName = m_filePath.filename().wstring();
    return fileName;
}

std::filesystem::path file::TilesetFile::GetFilePath() const
{
    return m_filePath;
}

bool file::TilesetFile::IsDirty() const
{
    return (
        !m_savedOrLoaded ||
        (m_tilesetDocument != nullptr && m_tilesetDocument->GetTilesetAssetId() != 0)
    );
}

void file::TilesetFile::SetFilePath(const std::filesystem::path& path)
{
    m_filePath = path;
}

std::vector<file::MapDocument*> file::TilesetFile::GetMapDocuments()
{
    return std::vector<MapDocument*>();
}

std::optional<file::MapDocument*> file::TilesetFile::GetMapDocument(size_t index)
{
    return std::nullopt;
}

std::vector<file::TilesetDocument*> file::TilesetFile::GetTilesetDocuments()
{
    if (m_tilesetDocument) {
        return std::vector<TilesetDocument*>{ m_tilesetDocument.get() };
    } else {
        return std::vector<TilesetDocument*>{};
    }
}

std::optional<file::TilesetDocument*> file::TilesetFile::GetTilesetDocument(size_t index)
{
    if (index == 0 && m_tilesetDocument) 
    {
        return m_tilesetDocument.get();
    }
    else 
    {
        return std::nullopt;
    }
}
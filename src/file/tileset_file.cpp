#include "file/tileset_file.hpp"
#include "defaults.hpp"

file::TilesetFile::TilesetFile(sgc::data::AssetId tilesetId) :
    m_tilesetDocument{std::make_unique<TilesetDocument>(tilesetId)}
{}

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

std::span<file::MapDocument*> file::TilesetFile::GetMapDocuments()
{
    return std::span<MapDocument*>();
}

std::span<file::TilesetDocument*> file::TilesetFile::GetTilesetDocuments()
{
    auto ptr = m_tilesetDocument.get();
    return std::span<TilesetDocument*>(&ptr, 1);
}
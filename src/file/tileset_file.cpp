#include "file/tileset_file.hpp"

file::TilesetFile::TilesetFile(sgc::data::AssetId tilesetId) :
    m_tilesetDocument{std::make_unique<TilesetDocument>(tilesetId)}
{}

void file::TilesetFile::Open(std::filesystem::path filePath){
    return;
}

void file::TilesetFile::Save(std::filesystem::path filePath){
    return;
}

void file::TilesetFile::Close(){
    return;
}

std::wstring file::TilesetFile::GetExtension() const
{
    auto extension = m_filePath.extension().wstring();
    return extension.empty() ? L"" : extension.substr(1);
}

std::wstring file::TilesetFile::GetFileName() const
{
    auto fileName = m_filePath.filename().wstring();
    return fileName;
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
#include "file/package_file.hpp"
#include <sgc/graphics/tileset.hpp>
#include <sgc/graphics/image.hpp>
#include <sgc/asset/tilesetasset.hpp>
#include <sgc/asset/imageasset.hpp>
#include <sgc/asset/assetheader.hpp>
#include <fstream>

file::PackageFile::PackageFile(std::wstring name, AssetManager& assetManager) :
    m_name(std::move(name)),
    m_assetManager(assetManager)
{
}

void file::PackageFile::AddMapDocument(std::unique_ptr<MapDocument> mapDocument)
{
    m_mapDocuments.push_back(std::move(mapDocument));
    m_fileAdded = true;
}

void file::PackageFile::AddTilesetDocument(std::unique_ptr<TilesetDocument> tilesetDocument)
{
    m_tilesetDocuments.push_back(std::move(tilesetDocument));
    m_fileAdded = true;
}

void file::PackageFile::Open()
{
    const uint8_t packageFileMagic[4] = { 'S', 'G', 'C', 'P' };


}

void file::PackageFile::Save()
{
    std::vector<uint8_t> bytes{};

    auto appendBytes = [&bytes](const std::vector<uint8_t>& data) {        
        bytes.insert(bytes.end(), data.begin(), data.end());
    };

    return;
}

void file::PackageFile::Close()
{
    //! as above
}

std::wstring file::PackageFile::GetExtension() const
{
    auto extension = m_filePath.extension().wstring();

    return (
        extension.empty() ?
        PackageFileExtension.data() :
        extension
    );
}

std::wstring file::PackageFile::GetFileName() const
{
    return m_filePath.filename().wstring();
}

std::filesystem::path file::PackageFile::GetFilePath() const
{
    return m_filePath;
}

bool file::PackageFile::IsDirty() const
{
    if(m_fileAdded) {
        return true;
    }

    for(const auto& mapDoc : m_mapDocuments) {
        if(mapDoc->IsDirty()) {
            return true;
        }
    }

    return false;
}

bool file::PackageFile::IsContainer() const
{
    return true;
}

bool file::PackageFile::IsActivable() const
{
    return false;
}

file::FileType file::PackageFile::GetFileType() const
{
    return FileType::Package;
}

file::ListableType file::PackageFile::GetListableType() const
{
    return ListableType::PackageFile;
}

void file::PackageFile::SetFilePath(const std::filesystem::path& path)
{
    m_filePath = path;
}

std::vector<file::MapDocument*> file::PackageFile::GetMapDocuments()
{
    std::vector<MapDocument*> mapDocs{};
    for(const auto& doc : m_mapDocuments) {
        mapDocs.push_back(doc.get());
    }
    return mapDocs;
}

std::optional<file::MapDocument*> file::PackageFile::GetMapDocument(size_t index)
{    
    if(index < m_mapDocuments.size() && index >= 0) {
        return m_mapDocuments[index].get();
    }
    return std::nullopt;
}

std::vector<file::TilesetDocument*> file::PackageFile::GetTilesetDocuments()
{
    std::vector<TilesetDocument*> tilesetDocs{};
    for(const auto& doc : m_tilesetDocuments) {
        tilesetDocs.push_back(doc.get());
    }
    return tilesetDocs;
}

std::optional<file::TilesetDocument*> file::PackageFile::GetTilesetDocument(size_t index)
{
    index -= m_mapDocuments.size(); // Adjust index to account for map documents and the package file itself
    if(index < m_tilesetDocuments.size() && index >= 0) {
        return m_tilesetDocuments[index].get();
    }
    return std::nullopt;
}

const std::wstring& file::PackageFile::GetName() const
{
    return m_name;
}
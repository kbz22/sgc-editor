#include "file/package_file.hpp"

file::PackageFile::PackageFile(std::wstring name) :
    m_name(std::move(name))
{
}

void file::PackageFile::AddMapDocument(std::unique_ptr<MapDocument> mapDocument)
{
    m_mapDocuments.push_back(std::move(mapDocument));
}

void file::PackageFile::AddTilesetDocument(std::unique_ptr<TilesetDocument> tilesetDocument)
{
    m_tilesetDocuments.push_back(std::move(tilesetDocument));
}

void file::PackageFile::Open()
{
    //! to do once the basics work
}

void file::PackageFile::Save()
{
    //! as above
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
    for(const auto& mapDoc : m_mapDocuments) {
        if(mapDoc->IsDirty()) {
            return true;
        }
    }

    for(const auto& tilesetDoc : m_tilesetDocuments) {
        if(tilesetDoc->IsDirty()) {
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
    if(index < m_mapDocuments.size()) {
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
    if(index < m_tilesetDocuments.size()) {
        return m_tilesetDocuments[index].get();
    }
    return std::nullopt;
}

const std::wstring& file::PackageFile::GetName() const
{
    return m_name;
}
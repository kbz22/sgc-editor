#include "file/package_file.hpp"
#include "program/except.hpp"
#include "sgc_extension/asset_builder.hpp"
#include "sgc_extension/asset_serializer.hpp"
#include "sgc_extension/runtime_builder.hpp"
#include "sgc_extension/asset_deserializer.hpp"
#include <sgc/graphics/tileset.hpp>
#include <sgc/graphics/image.hpp>
#include <sgc/asset/tilesetasset.hpp>
#include <sgc/asset/imageasset.hpp>
#include <sgc/asset/assetheader.hpp>
#include <sgc/data/package.hpp>
#include <sgc/data/packagebuilder.hpp>
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
    sgc::data::Package package{};

    if(!package.Open(m_filePath))
    {
        throw program::AssetLoadException("Failed to open package file: " + m_filePath.string());
    }


}

void file::PackageFile::Save()
{
    std::vector<uint8_t> bytes{};

    sgc::data::PackageBuilder packageBuilder{};
    
    for(const auto& mapDoc : m_mapDocuments) 
    {
        packageBuilder.AddAsset(
            mapDoc->GetMapAssetId(),
            sgc::data::AssetType::Map,
            sgc::asset::AssetSerializer<sgc::asset::MapAsset>::Serialize(
                sgc::asset::AssetBuilder<sgc::asset::MapAsset, file::MapDocument>::Build(*mapDoc)
        ));

        packageBuilder.AddAsset(
            mapDoc->GetMapDocumentAssetId(),
            sgc::data::AssetType::External,
            sgc::asset::AssetSerializer<file::MapDocumentInfo>::Serialize(
                sgc::asset::AssetBuilder<file::MapDocumentInfo, file::MapDocument>::Build(*mapDoc)
        ));
    }

    for(const auto& tilesetDoc : m_tilesetDocuments) 
    {
        // PackageBuilder uses a std::unordered_map internally, so adding the same asset multiple times will overwrite the previous
        //! if need be there might be a check here to only dump an image to the package once, since they contain the most data
        auto imageId = tilesetDoc->GetImageAssetId();
        packageBuilder.AddAsset(
            imageId,
            sgc::data::AssetType::Texture,
            sgc::asset::AssetSerializer<sgc::asset::ImageAsset>::Serialize(
                *m_assetManager.GetAsset<sgc::asset::ImageAsset>(imageId)
            )
        );

        auto tilesetId = tilesetDoc->GetTilesetAssetId();
        packageBuilder.AddAsset(
            tilesetId,
            sgc::data::AssetType::Tileset,
            sgc::asset::AssetSerializer<sgc::asset::TilesetAsset>::Serialize(
                *m_assetManager.GetAsset<sgc::asset::TilesetAsset>(tilesetId)
            )
        );
        
        packageBuilder.AddAsset(
            tilesetDoc->GetTilesetDocumentAssetId(),
            sgc::data::AssetType::External,
            sgc::asset::AssetSerializer<file::TilesetDocumentInfo>::Serialize(
                sgc::asset::AssetBuilder<file::TilesetDocumentInfo, file::TilesetDocument>::Build(*tilesetDoc)
        ));
    }

    auto packageData = packageBuilder.Build();

    std::ofstream outFile(m_filePath, std::ios::binary);
    if(outFile.is_open()) {
        outFile.write(reinterpret_cast<char*>(packageData.data()), packageData.size());
        outFile.close();
    }

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
#include "file/package_file.hpp"
#include "file/package_document.hpp"
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
    auto packageDocument = std::make_unique<PackageDocument>(*this);
    m_documents.push_back(std::move(packageDocument));
}

void file::PackageFile::AddMapDocument(std::unique_ptr<MapDocument> mapDocument)
{    
    m_mapDocuments.push_back(mapDocument.get());

    std::unique_ptr<IDocument> doc = std::move(mapDocument);    
    m_documents.push_back(std::move(doc));

    m_fileAdded = true;
}

void file::PackageFile::AddTilesetDocument(std::unique_ptr<TilesetDocument> tilesetDocument)
{
    m_tilesetDocuments.push_back(tilesetDocument.get());

    std::unique_ptr<IDocument> doc = std::move(tilesetDocument);
    m_documents.push_back(std::move(doc));
    
    m_fileAdded = true;
}

void file::PackageFile::Open()
{
    sgc::data::Package package{};

    if(!package.Open(m_filePath))
    {
        throw program::AssetLoadException("Failed to open package file: " + m_filePath.string());
    }

    std::vector<file::MapDocumentInfo> mapDocumentInfos{};
    std::vector<file::TilesetDocumentInfo> tilesetDocumentInfos{};

    for(auto &[id, entry] : package) 
    {
        switch(entry.type)
        {
            case sgc::data::AssetType::Map:
            {
                auto mapAsset = sgc::asset::AssetDeserializer<sgc::asset::MapAsset>::Deserialize(
                    package.ReadAssetData(id)
                );

                m_assetManager.AddAsset<sgc::asset::MapAsset>(
                    id,
                    std::make_shared<sgc::asset::MapAsset>(std::move(mapAsset))
                );

                break;
            }

            case sgc::data::AssetType::Tileset:
            {
                auto tilesetAsset = sgc::asset::AssetDeserializer<sgc::asset::TilesetAsset>::Deserialize(
                    package.ReadAssetData(id)
                );

                m_assetManager.AddAsset<sgc::asset::TilesetAsset>(
                    id,
                    std::make_shared<sgc::asset::TilesetAsset>(std::move(tilesetAsset))
                );

                break;
            }

            case sgc::data::AssetType::Texture:
            {
                auto imageAsset = sgc::asset::AssetDeserializer<sgc::asset::ImageAsset>::Deserialize(
                    package.ReadAssetData(id)
                );

                m_assetManager.AddAsset<sgc::asset::ImageAsset>(
                    id,
                    std::make_shared<sgc::asset::ImageAsset>(std::move(imageAsset))
                );

                break;
            }
                
            case sgc::data::AssetType::External:
            {
                auto entryData = package.ReadAssetData(id);
                auto externalType = static_cast<file::DocumentType>(entryData[0]);

                switch(externalType)
                {
                    default:
                        throw program::AssetLoadException("Unknown external document type: " + std::to_string(static_cast<int>(externalType)));

                    case file::DocumentType::Map:
                    {
                        mapDocumentInfos.push_back(
                            sgc::asset::AssetDeserializer<file::MapDocumentInfo>::Deserialize(entryData)
                        );
                        break;
                    }
                    case file::DocumentType::Tileset:
                    {
                        tilesetDocumentInfos.push_back(
                            sgc::asset::AssetDeserializer<file::TilesetDocumentInfo>::Deserialize(entryData)
                        );
                        break;
                    }
                }

                break;
            }

            default:
                continue;                
        }
    }

    for(const auto& mapDocInfo : mapDocumentInfos) 
    {
        AddMapDocument(
            sgc::asset::RuntimeBuilder<file::MapDocument>::Build(
                mapDocInfo,
                *m_assetManager.GetAsset<sgc::asset::MapAsset>(mapDocInfo.mapAssetId)
        ));
    }

    for(const auto& tilesetDocInfo : tilesetDocumentInfos) 
    {
        AddTilesetDocument(
            sgc::asset::RuntimeBuilder<file::TilesetDocument>::Build(
                tilesetDocInfo
        ));
    }

    m_fileAdded = false;
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

    for(auto &mapDoc : m_mapDocuments) {
        mapDoc->SetDirty(false);
    }
    m_fileAdded = false;

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
    /* std::vector<MapDocument*> mapDocs{};
    for(const auto& doc : m_mapDocuments) {
        mapDocs.push_back(doc.get());
    }
    return mapDocs; */
    return m_mapDocuments;
}

std::optional<file::MapDocument*> file::PackageFile::GetMapDocument(size_t index)
{
    if(index < m_mapDocuments.size() && index >= 0) {
        return m_mapDocuments[index];
    }
    return std::nullopt;
}

std::vector<file::TilesetDocument*> file::PackageFile::GetTilesetDocuments()
{
    /* std::vector<TilesetDocument*> tilesetDocs{};
    for(const auto& doc : m_tilesetDocuments) {
        tilesetDocs.push_back(doc.get());
    }
    return tilesetDocs; */
    return m_tilesetDocuments;
}

std::optional<file::TilesetDocument*> file::PackageFile::GetTilesetDocument(size_t index)
{
    // index -= m_mapDocuments.size(); // Adjust index to account for map documents and the package file itself
    // I'm refactoring this out and using generic interface for global indexes now, which makes sense not sure why I decided not to do that from the get-go
    if(index < m_tilesetDocuments.size() && index >= 0) {
        return m_tilesetDocuments[index];
    }
    return std::nullopt;
}

std::vector<file::IDocument*> file::PackageFile::GetDocuments()
{
    std::vector<IDocument*> documents{};
    for(auto &doc : m_documents) {
        documents.push_back(doc.get());
    }
    return documents;
}

std::optional<file::IDocument*> file::PackageFile::GetDocument(size_t index)
{
    if(index < m_documents.size() && index >= 0) {
        return m_documents[index].get();
    }    
    return std::nullopt;
}

const std::wstring& file::PackageFile::GetName() const
{
    return m_name;
}

void file::PackageFile::RegisterOnSetDirtyCallback(std::function<void(MapDocument*, bool)> callback)
{
    for(auto &mapDoc : m_mapDocuments) {
        mapDoc->RegisterOnSetDirtyCallback(callback);
    }
}

std::optional<size_t> file::PackageFile::GetDocumentIndex(IDocument *document)
{    
    auto documentCount = m_documents.size();
    for(int i = 0; i < documentCount; i++)
    {
        if(m_documents[i].get() == document)
        {
            return {i};
        }
    }

    return std::nullopt;
}
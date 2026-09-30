#include "file/tileset_file.hpp"
#include "file/document_info.hpp"
#include "sgc_extension/asset_serializer.hpp"
#include "sgc_extension/asset_deserializer.hpp"
#include "sgc_extension/asset_builder.hpp"
#include "sgc_extension/runtime_builder.hpp"
#include "program/except.hpp"
#include "defaults.hpp"
#include <sgc/graphics/tileset.hpp>
#include <sgc/graphics/image.hpp>
#include <sgc/data/resourcecontext.hpp>
#include <sgc/data/asset.hpp>
#include <sgc/data/package.hpp>
#include <sgc/data/packagebuilder.hpp>
#include <fstream>

file::TilesetFile::TilesetFile(AssetManager &assetManager)
    : m_assetManager(assetManager)
{
    return;
}

void file::TilesetFile::Open(){

    if(m_filePath.empty()) {
        return;
    }

    std::fstream file(m_filePath, std::ios::binary | std::ios::in);

    std::vector<uint8_t> bytes;

    file.seekg(0, std::ios::end);
    bytes.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(bytes.data()), bytes.size());

    std::vector<uint8_t> tilesetFileMagic = { 'S', 'G', 'C', 'T' };
    for(int i=0; i<tilesetFileMagic.size(); i++)
    {
        if(bytes[i] != tilesetFileMagic[i]) {
            throw program::AssetLoadException("Invalid tileset file format.");
        }
    }

    sgc::data::Package package;
    if(!package.Open(std::span<uint8_t>(bytes.data() + tilesetFileMagic.size(), bytes.size() - tilesetFileMagic.size()))) {
        throw program::AssetLoadException("Failed to open tileset package.");
    }

    sgc::data::AssetId tilesetDocumentAssetId = 0;
    sgc::data::AssetId tilesetAssetId = 0;
    sgc::data::AssetId imageAssetId = 0;

    for(auto &[id, entry] : package){
        switch(entry.type) 
        {
            case sgc::data::AssetType::External:
                tilesetDocumentAssetId = id;
                break;
            case sgc::data::AssetType::Tileset:
                tilesetAssetId = id;
                break;
            case sgc::data::AssetType::Texture:
                imageAssetId = id;
                break;
            default:
                break;
        }
    }

    if(tilesetDocumentAssetId == 0) {
        throw program::AssetLoadException("Tileset document asset not found in package.");
    }
    if(tilesetAssetId == 0) {
        throw program::AssetLoadException("Tileset asset not found in package.");
    }
    if(imageAssetId == 0) {
        throw program::AssetLoadException("Image asset not found in package.");
    }

    auto tilesetInfo = sgc::asset::AssetDeserializer<file::TilesetDocumentInfo>::Deserialize(
        package.ReadAssetData(tilesetDocumentAssetId)
    );

    m_tilesetDocument = sgc::asset::RuntimeBuilder<file::TilesetDocument>::Build(
        tilesetInfo
    );

    if(!m_assetManager.CheckAssetExists(imageAssetId)){
        auto imageAsset = std::make_shared<sgc::asset::ImageAsset>(
            sgc::asset::AssetDeserializer<sgc::asset::ImageAsset>::Deserialize(
                package.ReadAssetData(imageAssetId)
            )
        );
        m_assetManager.AddAsset(imageAssetId, imageAsset);
    }
    if(!m_assetManager.CheckAssetExists(tilesetAssetId)){
        auto tilesetAsset = std::make_shared<sgc::asset::TilesetAsset>(
            sgc::asset::AssetDeserializer<sgc::asset::TilesetAsset>::Deserialize(
                package.ReadAssetData(tilesetAssetId)
            )
        );
        m_assetManager.AddAsset(tilesetAssetId, tilesetAsset);
    }

    m_savedOrLoaded = true;

    return;
}

void file::TilesetFile::Save(){

    if(m_filePath.empty()) {
        return;
    }

    if(m_tilesetDocument == nullptr) {
        return;
    }

    sgc::data::PackageBuilder packageBuilder;

    auto imageId = m_tilesetDocument->GetImageAssetId();
    packageBuilder.AddAsset(
        imageId,
        sgc::data::AssetType::Texture,
        sgc::asset::AssetSerializer<sgc::asset::ImageAsset>::Serialize(
            *m_assetManager.GetAsset<sgc::asset::ImageAsset>(imageId)
        )
    );

    auto tilesetId = m_tilesetDocument->GetTilesetAssetId();
    packageBuilder.AddAsset(
        tilesetId,
        sgc::data::AssetType::Tileset,
        sgc::asset::AssetSerializer<sgc::asset::TilesetAsset>::Serialize(
            *m_assetManager.GetAsset<sgc::asset::TilesetAsset>(tilesetId)
        )
    );

    packageBuilder.AddAsset(
        m_tilesetDocument->GetTilesetDocumentAssetId(),
        sgc::data::AssetType::External,
        sgc::asset::AssetSerializer<file::TilesetDocumentInfo>::Serialize(
            sgc::asset::AssetBuilder<file::TilesetDocumentInfo, file::TilesetDocument>::Build(*m_tilesetDocument)
        )
    );    

    std::vector<uint8_t> tilesetFileMagic = { 'S', 'G', 'C', 'T' };
    std::vector<uint8_t> bytes = packageBuilder.Build();

    bytes.insert(bytes.begin(), tilesetFileMagic.begin(), tilesetFileMagic.end());

    std::ofstream file(m_filePath, std::ios::binary);
    file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    file.close();

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
        !(m_tilesetDocument != nullptr && m_tilesetDocument->GetTilesetAssetId() != 0)
    );
}

bool file::TilesetFile::IsContainer() const
{
    return false;
}

bool file::TilesetFile::IsActivable() const
{
    return false;
}

void file::TilesetFile::SetFilePath(const std::filesystem::path& path)
{
    m_filePath = path;
}

std::vector<file::MapDocument*> file::TilesetFile::GetMapDocuments()
{
    return std::vector<MapDocument*>();
}

std::optional<file::MapDocument*> file::TilesetFile::GetMapDocument([[maybe_unused]]size_t index)
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

std::vector<file::IDocument*> file::TilesetFile::GetDocuments()
{
    std::vector<IDocument*> documents{};
    if (m_tilesetDocument) {
        documents.push_back(
            reinterpret_cast<IDocument*>(m_tilesetDocument.get())
        );
    }
    return documents;
}

std::optional<file::IDocument*> file::TilesetFile::GetDocument(size_t index)
{
    if (m_tilesetDocument && index == 0)
    {
        return reinterpret_cast<IDocument*>(m_tilesetDocument.get());
    }
    else 
    {
        return std::nullopt;
    }
}

file::FileType file::TilesetFile::GetFileType() const
{
    return FileType::Tileset;
}

const std::wstring& file::TilesetFile::GetName() const
{
    return m_tilesetDocument->GetName();
}

file::ListableType file::TilesetFile::GetListableType() const
{
    return m_tilesetDocument->GetListableType();
}
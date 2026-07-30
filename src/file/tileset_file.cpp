#include "file/tileset_file.hpp"
#include "defaults.hpp"
#include "sgc/graphics/tileset.hpp"
#include "sgc/graphics/image.hpp"
#include "sgc/asset/tilesetasset.hpp"
#include "sgc/asset/imageasset.hpp"
#include "sgc/asset/tilesetassetserializer.hpp"
#include "sgc/asset/assetheaderdeserializer.hpp"
#include "sgc/data/resourcecontext.hpp"
#include "sgc/data/asset.hpp"
#include "program/except.hpp"
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

    for(int i = 0; i < tilesetFileMagic.size(); ++i) {
        if(bytes[i] != tilesetFileMagic[i]) {
            throw program::AssetLoadException("Invalid tileset file format.");
        }
    }

    size_t bytesIndex = tilesetFileMagic.size();
    
    auto imageHeader = sgc::asset::AssetDeserializer<sgc::data::AssetHeader>::Deserialize(
        std::vector<uint8_t>(bytes.begin() + bytesIndex, bytes.end())
    );

    bytesIndex += sizeof(sgc::data::AssetHeader);

    auto image = sgc::asset::AssetDeserializer<sgc::asset::ImageAsset>::Deserialize(
        std::vector<uint8_t>(bytes.begin() + bytesIndex, bytes.begin() + bytesIndex + imageHeader.size)
    );

    bytesIndex += imageHeader.size;

    auto tilesetHeader = sgc::asset::AssetDeserializer<sgc::data::AssetHeader>::Deserialize(
        std::vector<uint8_t>(bytes.begin() + bytesIndex, bytes.end())
    );

    bytesIndex += sizeof(sgc::data::AssetHeader);

    auto tileset = sgc::asset::AssetDeserializer<sgc::asset::TilesetAsset>::Deserialize(
        std::vector<uint8_t>(bytes.begin() + bytesIndex, bytes.begin() + bytesIndex + tilesetHeader.size)
    );

    std::shared_ptr<sgc::asset::ImageAsset> imagePtr = std::make_shared<sgc::asset::ImageAsset>(image);
    std::shared_ptr<sgc::asset::TilesetAsset> tilesetPtr = std::make_shared<sgc::asset::TilesetAsset>(tileset);
    
    if(!m_assetManager.CheckAssetExists(imageHeader.id)) {
        m_assetManager.AddAsset<sgc::asset::ImageAsset>(imageHeader.id, imagePtr);
    }

    if(!m_assetManager.CheckAssetExists(tilesetHeader.id)) {
        m_assetManager.AddAsset<sgc::asset::TilesetAsset>(tilesetHeader.id, tilesetPtr);
    }

    m_tilesetDocument = std::make_unique<TilesetDocument>(
        m_filePath.stem().wstring(),
        tilesetHeader.id,
        imageHeader.id
    );

    return;
}

void file::TilesetFile::Save(){

    if(m_filePath.empty()) {
        return;
    }

    if(m_tilesetDocument == nullptr) {
        return;
    }

    m_savedOrLoaded = true;

    auto imageAsset = m_assetManager.GetAsset<sgc::asset::ImageAsset>(
        m_tilesetDocument->GetImageAssetId()
    );
    
    auto tilesetAsset = m_assetManager.GetAsset<sgc::asset::TilesetAsset>(
        m_tilesetDocument->GetTilesetAssetId()
    );

    auto tilesetBytes = sgc::asset::AssetSerializer<sgc::asset::TilesetAsset>::Serialize(
        *tilesetAsset
    );    

    auto imageBytes = sgc::asset::AssetSerializer<sgc::asset::ImageAsset>::Serialize(
        *imageAsset
    );
    
    sgc::data::AssetHeader imageHeader = {
        m_tilesetDocument->GetImageAssetId(),
        sgc::data::AssetType::Texture,
        imageBytes.size()      
    };

    sgc::data::AssetHeader tilesetHeader = {
        m_tilesetDocument->GetTilesetAssetId(),
        sgc::data::AssetType::Tileset,
        tilesetBytes.size()
    };

    auto tilesetHeaderBytes = sgc::asset::AssetSerializer<sgc::data::AssetHeader>::Serialize(
        tilesetHeader
    );

    auto imageHeaderBytes = sgc::asset::AssetSerializer<sgc::data::AssetHeader>::Serialize(
        imageHeader
    );


    std::vector<uint8_t> tilesetFileMagic = { 'S', 'G', 'C', 'T' };

    std::vector<uint8_t> bytes;
    
    auto appendBytes = [&bytes](const std::vector<uint8_t>& data) {        
        bytes.insert(bytes.end(), data.begin(), data.end());
    };   

    appendBytes(tilesetFileMagic);
    appendBytes(imageHeaderBytes);
    appendBytes(imageBytes);
    appendBytes(tilesetHeaderBytes);
    appendBytes(tilesetBytes);

    std::ofstream file(m_filePath, std::ios::binary);
    file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());

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
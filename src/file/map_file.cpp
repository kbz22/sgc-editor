#include "file/map_file.hpp"
#include "defaults.hpp"
#include "program/except.hpp"
#include <sgc/asset/mapasset.hpp>
#include <sgc/asset/tilestorageasset.hpp>
#include <sgc/asset/chunkedtilestorageasset.hpp>
#include "sgc_extension/asset_builder.hpp"
#include "sgc_extension/runtime_builder.hpp"
#include "sgc_extension/asset_deserializer.hpp"
#include <sgc/data/packagebuilder.hpp>
#include <sgc/data/package.hpp>
#include <fstream>
#include <variant>

void file::MapFile::Open(){

    if(m_filePath.empty()) {
        return;
    }

    std::ifstream file(m_filePath, std::ios::binary);

    if(!file.is_open()) {
        throw program::FileLoadException("Failed to open map file: " + m_filePath.string());
    }

    std::vector<uint8_t> bytes;

    file.seekg(0, std::ios::end);
    bytes.resize(static_cast<size_t>(file.tellg()));
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(bytes.data()), bytes.size());

    file.close();

    std::vector<uint8_t> mapFileMagic = { 'S', 'G', 'C', 'M' };

    for(int i = 0; i < mapFileMagic.size(); ++i) {
        if(bytes[i] != mapFileMagic[i]) {
            throw program::AssetLoadException("Invalid map file format.");
        }
    }

    sgc::data::Package package;
    if(!package.Open(std::span<uint8_t>(bytes.begin()+mapFileMagic.size(), bytes.end()))) {
        throw program::AssetLoadException("Failed to open map package.");
    }

    sgc::data::AssetId mapAssetId = 0;
    sgc::data::AssetId mapDocId = 0;

    for(auto &[id, entry] : package) {
        if(entry.type == sgc::data::AssetType::Map) {
            mapAssetId = id;
        }        
        if(entry.type == sgc::data::AssetType::External) {
            mapDocId = id;
        }
    }

    if(mapAssetId == 0) {
        throw program::AssetLoadException("Map asset not found in package.");
    }

    auto mapAssetBytes = package.ReadAssetData(mapAssetId);
    auto mapAsset = sgc::asset::AssetDeserializer<sgc::asset::MapAsset>::Deserialize(
        mapAssetBytes
    );    

    if(mapDocId == 0) {
        throw program::AssetLoadException("Map document asset not found in package.");
    }

    auto docInfoBytes = package.ReadAssetData(mapDocId);
    auto docInfo = sgc::asset::AssetDeserializer<file::MapDocumentInfo>::Deserialize(
        docInfoBytes
    );

    m_document = sgc::asset::RuntimeBuilder<file::MapDocument>::Build(
        docInfo,
        mapAsset
    );

    return;
}

void file::MapFile::Save(){

    if(m_filePath.empty()) {
        return;
    }

    if(m_document == nullptr) {
        return;
    }
    
    sgc::data::PackageBuilder packageBuilder;

    packageBuilder.AddAsset(
        m_document->GetMapAssetId(),
        sgc::data::AssetType::Map,
        sgc::asset::AssetSerializer<sgc::asset::MapAsset>::Serialize(
            sgc::asset::AssetBuilder<sgc::asset::MapAsset, file::MapDocument>::Build(*m_document)
    ));

    packageBuilder.AddAsset(
        m_document->GetMapDocumentAssetId(),
        sgc::data::AssetType::External,
        sgc::asset::AssetSerializer<file::MapDocumentInfo>::Serialize(
            sgc::asset::AssetBuilder<file::MapDocumentInfo, file::MapDocument>::Build(*m_document)
    ));

    auto packageData = packageBuilder.Build();

    // insert extra magic bytes at the beginning to identify the file as a map file
    const uint8_t mapFileMagic[4] = { 'S', 'G', 'C', 'M' };
    packageData.insert(packageData.begin(), mapFileMagic, mapFileMagic + 4);

    std::ofstream file(m_filePath, std::ios::binary);
    file.write(reinterpret_cast<const char*>(packageData.data()), packageData.size());
    file.close();

    m_document->SetDirty(false);

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

file::FileType file::MapFile::GetFileType() const
{
    return FileType::Map;
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

bool file::MapFile::IsContainer() const
{
    return false;
}

bool file::MapFile::IsActivable() const
{
    return true;
}

std::vector<file::MapDocument*> file::MapFile::GetMapDocuments()
{    
    if (m_document) {
        return std::vector<MapDocument*>{ m_document.get() };
    } else {
        return std::vector<MapDocument*>{};
    }
}

std::optional<file::MapDocument*> file::MapFile::GetMapDocument(size_t index)
{
    if (m_document && index == 0) 
    {
        return m_document.get();
    }
    else
    {
        return std::nullopt;
    }
}

std::vector<file::TilesetDocument*> file::MapFile::GetTilesetDocuments()
{
    return std::vector<TilesetDocument*>{};
}

std::optional<file::TilesetDocument*> file::MapFile::GetTilesetDocument([[maybe_unused]] size_t index)
{
    return std::nullopt;
}

void file::MapFile::RegisterOnSetDirtyCallback(std::function<void(bool)> callback)
{
    if (m_document) {
        m_document->RegisterOnSetDirtyCallback(callback);
    }
}
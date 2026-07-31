#include "file/map_file.hpp"
#include "defaults.hpp"
#include "program/except.hpp"
#include <sgc/asset/mapasset.hpp>
#include <sgc/asset/mapassetserializer.hpp>
#include <sgc/asset/mapassetdeserializer.hpp>
#include <sgc/asset/tilestorageassetbuilder.hpp>
#include <sgc/asset/chunkedtilestoragebuilder.hpp>
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
    bytes.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(bytes.data()), bytes.size());

    file.close();

    std::vector<uint8_t> mapFileMagic = { 'S', 'G', 'C', 'M' };

    for(int i = 0; i < mapFileMagic.size(); ++i) {
        if(bytes[i] != mapFileMagic[i]) {
            throw program::AssetLoadException("Invalid map file format.");
        }
    }

    auto mapAsset = sgc::asset::AssetDeserializer<sgc::asset::MapAsset>::Deserialize(
        std::vector<uint8_t>(bytes.begin() + mapFileMagic.size(), bytes.end())
    );

    m_document = std::make_unique<MapDocument>(
        m_filePath.stem().wstring(),
        mapAsset.tilesetId
    );

    auto layerManager = m_document->GetLayerManager();

    for(const auto &layerAsset : mapAsset.layers) {
        std::shared_ptr<sgc::data::ITileStorage> tileStorage;

        if(std::holds_alternative<sgc::asset::ChunkedTileStorageAsset>(layerAsset.tileStorage)) {
            auto &chunkedStorage = std::get<sgc::asset::ChunkedTileStorageAsset>(layerAsset.tileStorage);
            tileStorage = sgc::asset::RuntimeBuilder<sgc::data::ChunkedTileStorage>::Build(chunkedStorage);
        } else {
            throw std::runtime_error("Unknown tile storage type");
        }

        program::LayerItem layerItem = {
            tileStorage,
            layerAsset.name,
            true,
            255
        };

        layerManager->AddLayer(layerItem);
    }

    return;
}

void file::MapFile::Save(){

    if(m_filePath.empty()) {
        return;
    }

    if(m_document == nullptr) {
        return;
    }

    auto layers = m_document->GetLayerManager()->GetLayers();

    std::vector<sgc::asset::MapLayerAsset> layerAssets;
    layerAssets.reserve(layers.size());

    for(auto &layer : layers) {
        sgc::asset::MapLayerAsset layerAsset = {
            layer.name,
            {},
            sgc::asset::AssetBuilder<sgc::asset::TileStorageAsset>::Build(*layer.storage)
        };

        layerAssets.push_back(layerAsset);
    }

    sgc::asset::MapAsset mapAsset = {
        m_document->GetTilesetAssetId(),
        layerAssets
    };

    auto bytes = sgc::asset::AssetSerializer<sgc::asset::MapAsset>::Serialize(
        mapAsset
    );

    uint8_t mapFileMagic[4] = { 'S', 'G', 'C', 'M' };

    bytes.insert(bytes.begin(), mapFileMagic, mapFileMagic + 4);

    std::ofstream file(m_filePath, std::ios::binary);
    file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());

    file.close();

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
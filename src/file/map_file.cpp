#include "file/map_file.hpp"
#include "defaults.hpp"
#include <sgc/asset/mapasset.hpp>
#include <sgc/asset/mapserializer.hpp>
#include <sgc/asset/tilestoragebuilder.hpp>
#include <fstream>

void file::MapFile::Open(){
    return;
}

void file::MapFile::Save(){

    if(m_filePath.empty()) {
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

std::optional<file::TilesetDocument*> file::MapFile::GetTilesetDocument(size_t index)
{
    return std::nullopt;
}
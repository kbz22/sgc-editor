#include "file/asset_manager.hpp"
#include "program/except.hpp"
#include <sgc/asset/tilesetasset.hpp>
#include <sgc/graphics/image.hpp>
#include <sgc/graphics/tileset.hpp>

std::shared_ptr<sgc::graphics::Tileset> file::AssetManager::MakeTileset(
    sgc::data::AssetId tilesetId,
    sgc::graphics::RenderContext* renderContext
)
{
    using namespace sgc::asset;
    using namespace sgc::graphics;

    if(m_cache.find(tilesetId) == m_cache.end()) {
        throw program::AssetLoadException("Tileset asset not found in the cache.");
    }

    std::shared_ptr<TilesetAsset> tilesetAsset;    
    tilesetAsset = std::get<std::shared_ptr<TilesetAsset>>(m_cache[tilesetId]);

    auto imageId = tilesetAsset->imageId;
    
    if(m_cache.find(imageId) == m_cache.end()) {
        throw program::AssetLoadException("Image asset not found in the cache.");
    }

    std::shared_ptr<Image> image = std::make_shared<Image>();
    auto imageAsset = std::get<std::shared_ptr<ImageAsset>>(m_cache[imageId]);

    if(image->LoadTexture(renderContext->renderer, imageAsset->data)) {
        // throw program::AssetLoadException("Failed to load texture for image asset.");
    }

    auto imageSize = image->GetSize();

    if(imageSize.x <= 0 || imageSize.y <= 0) {
        throw program::AssetLoadException("Invalid image dimensions for tileset asset.");
    }

    return std::make_shared<Tileset>(image, tilesetAsset->tileSize);
}

void file::AssetManager::AddAsset(
    sgc::data::AssetId assetId,
    std::shared_ptr<sgc::asset::ImageAsset> imageAsset
)
{
    if(m_cache.find(assetId) != m_cache.end()) {
        throw program::AssetLoadException("Asset with the same ID already exists in the cache.");
    }

    m_cache[assetId] = imageAsset;
}

void file::AssetManager::AddAsset(
    sgc::data::AssetId assetId,
    std::shared_ptr<sgc::asset::TilesetAsset> tilesetAsset
)
{
    if(m_cache.find(assetId) != m_cache.end()) {
        throw program::AssetLoadException("Asset with the same ID already exists in the cache.");
    }

    m_cache[assetId] = tilesetAsset;
}
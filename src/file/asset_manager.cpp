#include "file/asset_manager.hpp"
#include "program/except.hpp"
#include <sgc/asset/tilesetasset.hpp>
#include <sgc/graphics/image.hpp>
#include <sgc/graphics/tileset.hpp>

bool file::AssetManager::CheckAssetExists(sgc::data::AssetId assetId) const
{
    return m_cache.find(assetId) != m_cache.end();
}

std::shared_ptr<sgc::graphics::Tileset> file::AssetManager::MakeTileset(
    sgc::data::AssetId tilesetId,
    sgc::graphics::RenderContext* renderContext
)
{
    using namespace sgc::asset;
    using namespace sgc::graphics;

    if(m_cache.find(tilesetId) == m_cache.end()) {
        throw program::AssetCacheException("Tileset asset not found in the cache.");
    }

    std::shared_ptr<TilesetAsset> tilesetAsset;
    try {
        tilesetAsset = std::get<std::shared_ptr<TilesetAsset>>(m_cache.at(tilesetId));
    } catch (const std::bad_variant_access&) {
        throw program::AssetCacheException("Tileset asset not found in the cache.");
    }

    auto imageId = tilesetAsset->imageId;
    
    if(m_cache.find(imageId) == m_cache.end()) {
        throw program::AssetCacheException("Image asset not found in the cache.");
    }

    std::shared_ptr<Image> image = std::make_shared<Image>();
    auto imageAsset = std::get<std::shared_ptr<ImageAsset>>(m_cache[imageId]);

    if(!(image->LoadTexture(renderContext->renderer, imageAsset->data))) {
        throw program::AssetLoadException("Failed to load texture for image asset.");
    }

    auto imageSize = image->GetSize();

    if(imageSize.x <= 0 || imageSize.y <= 0) {
        throw program::AssetLoadException("Invalid image dimensions for tileset asset.");
    }

    return std::make_shared<Tileset>(image, tilesetAsset->tileSize);
}

bool file::AssetManager::RemoveAsset(sgc::data::AssetId assetId)
{
    if(m_cache.find(assetId) == m_cache.end()) {
        return false;
    }

    m_cache.erase(assetId);
    return true;
}

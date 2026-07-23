#include "file/asset_manager.hpp"
#include "program/except.hpp"

file::AssetManager::AssetManager()
{
    m_imageCache = std::unordered_map<sgc::data::AssetId, std::shared_ptr<sgc::graphics::Image>>();
}

std::shared_ptr<sgc::graphics::Tileset> file::AssetManager::LoadTileset(
    sgc::data::AssetId assetId,
    const std::filesystem::path& path,
    int tileWidth,
    int tileHeight,
    sgc::graphics::RenderContext* renderContext
)
{
    if(tileWidth <= 0 || tileHeight <= 0) {
        throw program::TileSizeException("Tile size must be greater than zero.");
    }

    sgc::graphics::PixelSize2D tileSize{ tileWidth, tileHeight };

    /* if(m_imageCache.find(assetId) != m_imageCache.end()) {
        auto image = m_imageCache[assetId];
        return std::make_shared<sgc::graphics::Tileset>(image, tileSize);
    } */

    auto image = std::make_shared<sgc::graphics::Image>();

    image->LoadTexture(renderContext->renderer, path);

    auto imageSize = image->GetSize();

    if(!image->LoadTexture(renderContext->renderer, path)) {
        throw program::AssetLoadException("Failed to load tileset image: " + path.string());
    }

    if (imageSize.x == 0 || imageSize.y == 0) {
        throw program::AssetLoadException("Tileset image has invalid dimensions: " + path.string());
    }

    if (imageSize.x % tileWidth != 0 || imageSize.y % tileHeight != 0) {
        throw program::TileSizeException("Tileset dimensions are not divisible by tile size.");
    }

    if (imageSize.x < tileWidth || imageSize.y < tileHeight) {
        throw program::TileSizeException("Image is smaller than the specified tile size.");
    }  

    m_imageCache[assetId] = image;

    return std::make_shared<sgc::graphics::Tileset>(image, tileSize);
}
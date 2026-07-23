#pragma once

#include <sgc/data/asset.hpp>
#include <sgc/graphics/tileset.hpp>
#include <sgc/graphics/image.hpp>
#include <sgc/graphics/rendercontext.hpp>
#include <memory>
#include <unordered_map>

namespace file {

    class AssetManager
    {
        private:
            std::unordered_map<
                sgc::data::AssetId,
                std::shared_ptr<sgc::graphics::Image>
            > m_imageCache;

        public:
            AssetManager();
            virtual ~AssetManager() = default;

            std::shared_ptr<sgc::graphics::Tileset> LoadTileset(
                sgc::data::AssetId assetId,
                const std::filesystem::path& path,
                int tileWidth,
                int tileHeight,
                sgc::graphics::RenderContext* renderContext
            );
    };

}
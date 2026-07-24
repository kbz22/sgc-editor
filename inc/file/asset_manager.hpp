#pragma once

#include <sgc/data/asset.hpp>
#include <sgc/asset/imageasset.hpp>
#include <sgc/asset/tilesetasset.hpp>
#include <sgc/graphics/tileset.hpp>
#include <sgc/graphics/image.hpp>
#include <sgc/graphics/rendercontext.hpp>
#include <memory>
#include <unordered_map>
#include <variant>

namespace file {

    class AssetManager
    {
        private:
            std::unordered_map<
                sgc::data::AssetId,
                std::variant<
                    std::shared_ptr<sgc::asset::ImageAsset>,
                    std::shared_ptr<sgc::asset::TilesetAsset>
                >
            > m_cache{};

        public:
            AssetManager() = default;
            virtual ~AssetManager() = default;

            void AddAsset(
                sgc::data::AssetId assetId,
                std::shared_ptr<sgc::asset::ImageAsset> imageAsset
            );
            void AddAsset(
                sgc::data::AssetId assetId,
                std::shared_ptr<sgc::asset::TilesetAsset> tilesetAsset
            );

            std::shared_ptr<sgc::graphics::Tileset> MakeTileset(
                sgc::data::AssetId assetId,
                sgc::graphics::RenderContext* renderContext
            );
    };

}
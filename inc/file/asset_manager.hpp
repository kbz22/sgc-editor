#pragma once

#include <sgc/data/asset.hpp>
#include <sgc/asset/imageasset.hpp>
#include <sgc/asset/tilesetasset.hpp>
#include <sgc/asset/mapasset.hpp>
#include <sgc/graphics/tileset.hpp>
#include <sgc/graphics/image.hpp>
#include <sgc/graphics/rendercontext.hpp>
#include <memory>
#include <unordered_map>
#include <variant>

namespace file {

    using AssetType = std::variant<
        std::shared_ptr<sgc::asset::ImageAsset>,
        std::shared_ptr<sgc::asset::TilesetAsset>,
        std::shared_ptr<sgc::asset::MapAsset>
    >;

    class AssetManager
    {
        private:
            std::unordered_map< sgc::data::AssetId, AssetType > m_cache{};

        public:
            AssetManager() = default;
            virtual ~AssetManager() = default;

            template<typename T>
            void AddAsset(sgc::data::AssetId assetId, std::shared_ptr<T> asset);

            template<typename T>
            std::shared_ptr<T> GetAsset(sgc::data::AssetId assetId) const;

            bool CheckAssetExists(sgc::data::AssetId assetId) const;
            bool RemoveAsset(sgc::data::AssetId assetId);

            std::shared_ptr<sgc::graphics::Tileset> MakeTileset(
                sgc::data::AssetId assetId,
                sgc::graphics::RenderContext* renderContext
            );
    };

}
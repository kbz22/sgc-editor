#pragma once

#include "program/except.hpp"
#include <sgc/data/asset.hpp>
#include <sgc/asset/assets.hpp>
#include <sgc/graphics/tileset.hpp>
#include <sgc/graphics/rendercontext.hpp>
#include <memory>
#include <unordered_map>
#include <variant>

namespace file {

    class AssetManager
    {
        private:
            std::unordered_map< sgc::data::AssetId, sgc::asset::AssetVariant > m_cache{};

        public:
            AssetManager() = default;
            virtual ~AssetManager() = default;

            template<typename AssetT>
            void AddAsset(sgc::data::AssetId assetId, std::shared_ptr<AssetT> asset)
            {
                m_cache[assetId] = asset;
            }

            template<typename AssetT>
            std::shared_ptr<AssetT> GetAsset(sgc::data::AssetId assetId) const
            {
                if(m_cache.find(assetId) == m_cache.end())
                {
                    throw program::AssetCacheException("Map asset not found in the cache.");
                }

                return std::get<std::shared_ptr<AssetT>>(m_cache.at(assetId));
            }

            bool CheckAssetExists(sgc::data::AssetId assetId) const;
            bool RemoveAsset(sgc::data::AssetId assetId);

            std::shared_ptr<sgc::graphics::Tileset> MakeTileset(
                sgc::data::AssetId assetId,
                sgc::graphics::RenderContext* renderContext
            );
    };

}
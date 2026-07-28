#pragma once

#include "program/layer_manager.hpp"
#include <sgc/graphics/tileset.hpp>
#include <sgc/data/itilestorage.hpp>
#include <sgc/data/asset.hpp>

#include <memory>

namespace file {

    class MapDocument
    {
        private:
            std::unique_ptr<program::LayerManager> m_layerManager{nullptr};
            sgc::data::AssetId m_tilesetId{0};
            std::wstring name;
            bool m_dirty{false};

        public:
            MapDocument(std::wstring name, sgc::data::AssetId tilesetId);
            virtual ~MapDocument() = default;

            program::LayerManager* GetLayerManager();
            sgc::data::ITileStorage* GetCurrentLayerStorage();
            sgc::data::AssetId GetTilesetAssetId();

            bool IsEditable() const;
            bool IsDirty() const;

            void SetDirty(bool dirty);
    };

}
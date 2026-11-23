#pragma once

#include "program/layer_manager.hpp"
#include "program/tile_symbol_manager.hpp"
#include "file/itreeviewlistable.hpp"
#include "file/idocument.hpp"
#include <sgc/graphics/tileset.hpp>
#include <sgc/data/itilestorage.hpp>
#include <sgc/data/asset.hpp>
#include <sgc/asset/metadataasset.hpp>
#include "command/command_manager.hpp"

#include <memory>
#include <functional>

namespace file
{
    using sgc::asset::AssetMetadata;

    class MapDocument : public IDocument
    {
        private:
            std::unique_ptr<program::LayerManager> m_layerManager{nullptr};
            std::unique_ptr<command::CommandManager> m_commandManager{nullptr};
            std::unique_ptr<program::TileSymbolManager> m_symbolManager{nullptr};
            sgc::data::AssetId m_tilesetId{0};
            sgc::data::AssetId m_mapAssetId{0};
            sgc::data::AssetId m_mapDocumentId{0};
            sgc::data::AssetId m_symbolGroupsId{0};
            sgc::data::AssetId m_metadataAssetId{0};
            std::wstring m_name;
            bool m_dirty{false};
            std::function<void(MapDocument*, bool)> m_onSetDirtyCallback{nullptr};

        public:
            MapDocument(std::wstring name, sgc::data::AssetId tilesetId);
            MapDocument(std::wstring name, sgc::data::AssetId tilesetId, sgc::data::AssetId mapAssetId);
            MapDocument(std::wstring name, sgc::data::AssetId tilesetId, sgc::data::AssetId mapAssetId, sgc::data::AssetId symbolGroupsId);
            MapDocument(std::wstring name, sgc::data::AssetId tilesetId, sgc::data::AssetId mapAssetId, sgc::data::AssetId symbolGroupsId, sgc::data::AssetId metadataAssetId);
            MapDocument(const MapDocument&);

            program::LayerManager* GetLayerManager() const;
            command::CommandManager* GetCommandManager() const;
            program::TileSymbolManager* GetSymbolManager() const;
            sgc::data::ITileStorage* GetCurrentLayerStorage() const;
            sgc::data::AssetId GetTilesetAssetId() const;
            sgc::data::AssetId GetMapAssetId() const;
            sgc::data::AssetId GetMapDocumentAssetId() const;
            sgc::data::AssetId GetSymbolGroupsAssetId() const;
            sgc::data::AssetId GetMetadataAssetId() const;

            AssetMetadata GenerateMetadataAsset() const;

            bool IsContainer() const override;            
            bool IsDirty() const override;
            bool IsActivable() const override;
            const std::wstring& GetName() const override;
            ListableType GetListableType() const override;            

            bool IsEditable() const;
            void SetDirty(bool dirty);
            void RegisterOnSetDirtyCallback(std::function<void(MapDocument*, bool)> callback);            
    };
}
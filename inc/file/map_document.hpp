#pragma once

#include "program/layer_manager.hpp"
#include "file/itreeviewlistable.hpp"
#include <sgc/graphics/tileset.hpp>
#include <sgc/data/itilestorage.hpp>
#include <sgc/data/asset.hpp>
#include "command/command_manager.hpp"

#include <memory>
#include <functional>

namespace file {

    class MapDocument : public ITreeViewListable
    {
        private:
            std::unique_ptr<program::LayerManager> m_layerManager{nullptr};
            std::unique_ptr<command::CommandManager> m_commandManager{nullptr};
            sgc::data::AssetId m_tilesetId{0};
            std::wstring name;
            bool m_dirty{false};
            std::function<void(bool)> m_onSetDirtyCallback{nullptr};

        public:
            MapDocument(std::wstring name, sgc::data::AssetId tilesetId);
            virtual ~MapDocument() = default;

            program::LayerManager* GetLayerManager();
            command::CommandManager* GetCommandManager();
            sgc::data::ITileStorage* GetCurrentLayerStorage();
            sgc::data::AssetId GetTilesetAssetId();

            bool IsContainer() const override;            
            bool IsDirty() const override;
            bool IsActivable() const override;
            const std::wstring& GetName() const override;
            ListableType GetListableType() const override;            

            bool IsEditable() const;
            void SetDirty(bool dirty);
            void RegisterOnSetDirtyCallback(std::function<void(bool)> callback);            
    };
}
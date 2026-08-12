#pragma once

#include <sgc/data/itilestorage.hpp>
#include <sgc/graphics/tiledimage.hpp>

#include <vector>
#include <string>

namespace program {

    struct LayerItem
    {
        std::shared_ptr<sgc::data::ITileStorage> storage;
        std::wstring name;
        bool visible = true;
        uint8_t transparency = 255;
    };

    class LayerManager
    {
        private:
            std::vector<LayerItem> m_layers;
            size_t m_activeLayerIndex = 0;
            size_t m_baseLayerIndex = 0;
            bool m_singleLayerMode = false;

        public:
            LayerManager() = default;
            ~LayerManager() = default;
            
            size_t AddLayer(LayerItem entry);
            size_t InsertLayer(LayerItem entry, size_t index);
            LayerItem RemoveLayer(size_t index);
            void MoveLayer(size_t fromIndex, int movement);
            void MoveActiveLayer(int movement);
            
            void SetActiveLayerIndex(size_t index);
            void SetBaseLayerIndex(size_t index);
            void SetLayerVisibility(size_t index, bool visible);
            void SetLayerTransparency(size_t index, uint8_t transparency);
            void SetSingleLayerMode(bool singleLayerMode);
            void SetLayerName(size_t index, std::wstring name);

            std::vector<LayerItem> GetLayers() const;
            size_t GetActiveLayerIndex() const;
            size_t GetBaseLayerIndex() const;
            size_t GetSize() const;
            bool IsSingleLayerMode() const;
    };

}
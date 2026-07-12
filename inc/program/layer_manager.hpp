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
    };

    class LayerManager
    {
        private:
            std::vector<LayerItem> m_layers;
            size_t m_activeLayerIndex = 0;
            size_t m_baseLayerIndex = 0;

        public:
            LayerManager() = default;
            ~LayerManager() = default;
            
            void AddLayer(LayerItem entry);
            void InsertLayer(LayerItem entry, size_t index);
            void RemoveLayer(size_t index);
            void SetActiveLayerIndex(size_t index);
            void SetBaseLayerIndex(size_t index);     

            std::vector<LayerItem> GetLayers() const;
            size_t GetActiveLayerIndex() const;
            size_t GetBaseLayerIndex() const;
    };        

}
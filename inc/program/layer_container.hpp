#pragma once

#include <sgc/data/itilestorage.hpp>
#include <sgc/graphics/tiledimage.hpp>

#include <vector>
#include <string>

namespace program {

    struct LayerEntry
    {
        std::shared_ptr<sgc::data::ITileStorage> storage;
        std::shared_ptr<sgc::graphics::TiledImage> layer;
        std::wstring name;
    };

    class LayerContainer
    {
        private:
            std::vector<LayerEntry> m_layers;
            size_t m_activeLayerIndex = 0;
            size_t m_baseLayerIndex = 0;

        public:
            LayerContainer() = default;
            ~LayerContainer() = default;
            
            void AddLayer(LayerEntry entry);
            void InsertLayer(LayerEntry entry, size_t index);
            void RemoveLayer(size_t index);
            void SetActiveLayerIndex(size_t index);            

            std::vector<LayerEntry> GetLayers() const;
            size_t GetActiveLayerIndex() const;
            size_t GetBaseLayerIndex() const;
    };        

}
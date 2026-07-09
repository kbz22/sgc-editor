#pragma once

#include <sgc/data/itilestorage.hpp>
#include <sgc/graphics/tiledimage.hpp>
#include <vector>

namespace program {

    struct LayerEntry
    {
        std::shared_ptr<sgc::data::ITileStorage> storage;
        std::shared_ptr<sgc::graphics::TiledImage> layer;
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
    };        

}
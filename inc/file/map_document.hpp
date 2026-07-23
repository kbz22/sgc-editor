#pragma once

#include "program/layer_manager.hpp"
#include <sgc/graphics/tileset.hpp>
#include <sgc/data/itilestorage.hpp>

#include <memory>

namespace file {

    class MapDocument
    {
        private:
            std::unique_ptr<program::LayerManager> m_layerManager{nullptr};            

        public:
            MapDocument();
            virtual ~MapDocument() = default;

            program::LayerManager* GetLayerManager();
            sgc::data::ITileStorage* GetCurrentLayerStorage();
            sgc::graphics::Tileset* GetTileset();
    };

}
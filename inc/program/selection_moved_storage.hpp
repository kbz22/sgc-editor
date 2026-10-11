#pragma once

#include <memory>
#include <map>
#include <sgc/data/statictilestorage.hpp>

namespace program
{
    struct SelectionMovedLayerData 
    {
        std::shared_ptr<sgc::data::StaticTileStorage> storage;
        uint8_t transparency;
        size_t renderingIndex;
    };

    using SelectionMovedStorage = std::map<size_t, SelectionMovedLayerData>;
}
#include "file/map_document.hpp"
#include <sgc/data/helpers.hpp>

file::MapDocument::MapDocument() :
    m_layerManager{std::make_unique<program::LayerManager>()}
{}

program::LayerManager* file::MapDocument::GetLayerManager()
{
    return m_layerManager.get();
}

sgc::data::ITileStorage* file::MapDocument::GetCurrentLayerStorage()
{
    auto currentLayerIndex = m_layerManager->GetActiveLayerIndex();
    return m_layerManager->GetLayers()[currentLayerIndex].storage.get();
}
#include "file/map_document.hpp"

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

/* sgc::graphics::Tileset& file::MapDocument::GetTileset()
{
    return m_tileset;
} */
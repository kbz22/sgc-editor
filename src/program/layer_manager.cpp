#include "program/layer_manager.hpp"
#include <stdexcept>

void program::LayerManager::AddLayer(LayerItem entry)
{    
    // m_layers.push_back(entry);
    m_layers.insert(m_layers.begin() + m_activeLayerIndex, entry);
}

void program::LayerManager::InsertLayer(LayerItem entry, size_t index)
{
    if (index > m_layers.size()) {
        throw std::out_of_range("Index is out of range for inserting layer.");
    }

    m_layers.insert(m_layers.begin() + index, entry);
}

void program::LayerManager::RemoveLayer(size_t index)
{
    if(index >= m_layers.size()) {
        throw std::out_of_range("Index is out of range for removing layer.");
    }
    
    if(index <= m_activeLayerIndex) {
        if(m_activeLayerIndex > 0) {
            --m_activeLayerIndex;
        } else {
            m_activeLayerIndex = 0;
        }
    }

    if(index <= m_baseLayerIndex) {
        if(m_baseLayerIndex > 0) {
            --m_baseLayerIndex;
        } else {
            m_baseLayerIndex = 0;
        }
    }

    m_layers.erase(m_layers.begin() + index);
}

void program::LayerManager::SetActiveLayerIndex(size_t index)
{
    if (index >= m_layers.size()) {
        throw std::out_of_range("Index is out of range for setting active layer.");
    }

    m_activeLayerIndex = index;
}

void program::LayerManager::SetBaseLayerIndex(size_t index)
{
    if (index >= m_layers.size()) {
        throw std::out_of_range("Index is out of range for setting base layer.");
    }

    m_baseLayerIndex = index;
}

std::vector<program::LayerItem> program::LayerManager::GetLayers() const
{
    return m_layers;
}

size_t program::LayerManager::GetActiveLayerIndex() const
{
    return m_activeLayerIndex;
}

size_t program::LayerManager::GetBaseLayerIndex() const
{
    return m_baseLayerIndex;
}
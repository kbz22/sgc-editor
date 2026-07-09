#include "program/layer_container.hpp"
#include <stdexcept>

void program::LayerContainer::AddLayer(LayerEntry entry)
{
    m_layers.push_back(entry);
}

void program::LayerContainer::InsertLayer(LayerEntry entry, size_t index)
{
    if (index > m_layers.size()) {
        throw std::out_of_range("Index is out of range for inserting layer.");
    }

    m_layers.insert(m_layers.begin() + index, entry);
}

void program::LayerContainer::RemoveLayer(size_t index)
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

void program::LayerContainer::SetActiveLayerIndex(size_t index)
{
    if (index >= m_layers.size()) {
        throw std::out_of_range("Index is out of range for setting active layer.");
    }

    m_activeLayerIndex = index;
}

size_t program::LayerContainer::GetActiveLayerIndex() const
{
    return m_activeLayerIndex;
}

size_t program::LayerContainer::GetBaseLayerIndex() const
{
    return m_baseLayerIndex;
}
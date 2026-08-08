#include "program/layer_manager.hpp"
#include <stdexcept>

size_t program::LayerManager::AddLayer(LayerItem entry)
{    
    // m_layers.push_back(entry);
    m_layers.insert(m_layers.begin() + m_activeLayerIndex, entry);

    return m_activeLayerIndex;
}

size_t program::LayerManager::InsertLayer(LayerItem entry, size_t index)
{
    if (index > m_layers.size()) {
        throw std::out_of_range("Index is out of range for inserting layer.");
    }

    m_layers.insert(m_layers.begin() + index, entry);

    return index;
}

std::shared_ptr<sgc::data::ITileStorage> program::LayerManager::RemoveLayer(size_t index)
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

    auto storage = m_layers[index].storage;
    m_layers.erase(m_layers.begin() + index);

    return storage;
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

void program::LayerManager::SetLayerVisibility(size_t index, bool visible)
{
    if (index >= m_layers.size()) {
        throw std::out_of_range("Index is out of range for setting layer visibility.");
    }

    m_layers[index].visible = visible;
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

void program::LayerManager::MoveActiveLayer(int movement)
{
    MoveLayer(m_activeLayerIndex, movement);

    m_activeLayerIndex = static_cast<size_t>(static_cast<int>(m_activeLayerIndex) + movement);

    if(m_activeLayerIndex >= m_layers.size()) {
        m_activeLayerIndex = m_layers.size() - 1;
    }

    if(m_activeLayerIndex < 0) {
        m_activeLayerIndex = 0;
    }
}

void program::LayerManager::MoveLayer(size_t fromIndex, int movement)
{
    if (fromIndex >= m_layers.size()) {
        throw std::out_of_range("fromIndex is out of range for moving layer.");
    }

    int toIndex = static_cast<int>(fromIndex) + movement;

    if (toIndex < 0 || toIndex >= static_cast<int>(m_layers.size())) {
        throw std::out_of_range("toIndex is out of range for moving layer.");
    }

    auto layer = m_layers[fromIndex];
    m_layers.erase(m_layers.begin() + fromIndex);
    m_layers.insert(m_layers.begin() + toIndex, layer);
}

size_t program::LayerManager::GetSize() const
{
    return m_layers.size();
}

void program::LayerManager::SetSingleLayerMode(bool singleLayerMode)
{
    m_singleLayerMode = singleLayerMode;
}

void program::LayerManager::SetLayerName(size_t index, std::wstring name)
{
    if (index >= m_layers.size()) {
        throw std::out_of_range("Index is out of range for setting layer name.");
    }

    m_layers[index].name = name;
}

void program::LayerManager::SetLayerTransparency(size_t index, uint8_t transparency)
{
    if (index >= m_layers.size()) {
        throw std::out_of_range("Index is out of range for setting layer transparency.");
    }

    m_layers[index].transparency = transparency;
}

bool program::LayerManager::IsSingleLayerMode() const
{
    return m_singleLayerMode;
}
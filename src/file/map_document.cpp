#include "file/map_document.hpp"
#include <sgc/data/helpers.hpp>
#include "program/program.hpp"

file::MapDocument::MapDocument(std::wstring name, sgc::data::AssetId tilesetId) :
    name{name},
    m_layerManager{std::make_unique<program::LayerManager>()},
    m_tilesetId{tilesetId}
{}

program::LayerManager* file::MapDocument::GetLayerManager()
{
    return m_layerManager.get();
}

sgc::data::ITileStorage* file::MapDocument::GetCurrentLayerStorage()
{
    auto currentLayerIndex = m_layerManager->GetActiveLayerIndex();
    auto layers = m_layerManager->GetLayers();

    if(layers.empty() || currentLayerIndex >= layers.size()) {
        return nullptr;
    }

    return m_layerManager->GetLayers()[currentLayerIndex].storage.get();
}

sgc::data::AssetId file::MapDocument::GetTilesetAssetId()
{
    return m_tilesetId;
}

bool file::MapDocument::IsEditable() const
{
    program::ProgramContext& programContext = program::GetProgramContext();
    return programContext.assetManager->CheckAssetExists(m_tilesetId);
}

bool file::MapDocument::IsDirty() const
{
    return m_dirty;
}

void file::MapDocument::SetDirty(bool dirty)
{
    m_dirty = dirty;
}

const std::wstring& file::MapDocument::GetName() const
{
    return name;
}
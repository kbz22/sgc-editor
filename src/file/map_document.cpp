#include "file/map_document.hpp"
#include <sgc/data/helpers.hpp>
#include "program/program.hpp"

file::MapDocument::MapDocument(std::wstring name, sgc::data::AssetId tilesetId) :
    name{name},
    m_layerManager{std::make_unique<program::LayerManager>()},
    m_commandManager{std::make_unique<command::CommandManager>()},
    m_tilesetId{tilesetId}
{}

file::MapDocument::MapDocument(const MapDocument& other) :
    name{other.name},
    m_layerManager{std::make_unique<program::LayerManager>(*other.m_layerManager)},
    m_commandManager{std::make_unique<command::CommandManager>(*other.m_commandManager)},
    m_tilesetId{other.m_tilesetId},
    m_dirty{other.m_dirty},
    m_onSetDirtyCallback{other.m_onSetDirtyCallback}
{}

program::LayerManager* file::MapDocument::GetLayerManager()
{
    return m_layerManager.get();
}

command::CommandManager* file::MapDocument::GetCommandManager()
{
    return m_commandManager.get();
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
    if(m_onSetDirtyCallback) {
        m_onSetDirtyCallback(dirty);
    }
}

void file::MapDocument::RegisterOnSetDirtyCallback(std::function<void(bool)> callback)
{
    m_onSetDirtyCallback = callback;
}

const std::wstring& file::MapDocument::GetName() const
{
    return name;
}

bool file::MapDocument::IsContainer() const
{
    return false;
}

bool file::MapDocument::IsActivable() const
{
    return true;
}

file::ListableType file::MapDocument::GetListableType() const
{
    return ListableType::MapDocument;
}
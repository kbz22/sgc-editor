#include "sections/map_section.hpp"
#include <sgc/data/resourcemanager.hpp>
#include <sgc/asset/assetloader.hpp>
#include <sgc/asset/chunkedtilestorageserializer.hpp>
#include <sgc/asset/chunkedtilestorageassetbuilder.hpp>
#include <fstream>
#include <filesystem>

sections::MapSection::MapSection(win32_program::Win32Context& context) :
    Section{L"MapView", win32_program::ControlId::MapView, context},
    m_mapView{nullptr}
{}

void sections::MapSection::Update(program::EditorState state)
{
    if (m_mapView != nullptr) {
        m_mapView->Render();
    }    
}

void sections::MapSection::LoadTileset(const std::filesystem::path& path, int tileWidth, int tileHeight)
{   
    if(m_mapView != nullptr) {
        m_mapView.reset();
    }
    
    m_mapView = std::make_unique<sgc_view::MapView>(GetHwnd(), tileWidth, tileHeight);
    m_mapView->LoadTileset(path.wstring());
}

void sections::MapSection::LoadMap(const std::filesystem::path& path)
{
    if (m_mapView == nullptr) {
        return;
    }

    std::ifstream file(path, std::ios::binary);
    std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    sgc::data::ResourceContext rc;
    sgc::data::ResourceManager rm;

    auto storage = sgc::asset::AssetLoader<sgc::data::ChunkedTileStorage>::Load(
        bytes,
        rc,
        rm
    );    

    m_mapView->SetStorage(
        std::make_shared<sgc::data::ChunkedTileStorage>(std::move(*storage))
    );
}

void sections::MapSection::SaveMap(const std::filesystem::path& path)
{
    if (m_mapView == nullptr) {
        return;
    }    

    auto asset = sgc::asset::AssetBuilder<sgc::asset::ChunkedTileStorageAsset>::Build(        
        *(m_mapView->m_tileStorage)
    );

    auto bytes = sgc::asset::AssetSerializer<sgc::asset::ChunkedTileStorageAsset>::Serialize(
        asset
    );

    std::ofstream file(path, std::ios::binary);
    file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());

}

void sections::MapSection::HandleSectionResize()
{
    if (m_mapView != nullptr) {
        RECT rect;
        GetClientRect(GetHwnd(), &rect);
        m_mapView->SetScreenSize(rect.right - rect.left, rect.bottom - rect.top);        
    }
    Update(program::EditorState::Resized);
}
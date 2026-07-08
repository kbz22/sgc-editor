#include "sections/tileset_section.hpp"

sections::TilesetSection::TilesetSection(win32_program::Win32Context& context) :
    Section{L"TilesetView", win32_program::ControlId::TilesetView, context},
    m_tilesetView{nullptr}
{}

void sections::TilesetSection::Update()
{
    if (m_tilesetView != nullptr) {
        m_tilesetView->Render();
    }   
}

void sections::TilesetSection::LoadTileset(const std::filesystem::path& path, int tileWidth, int tileHeight)
{   
    if(m_tilesetView != nullptr) {
        m_tilesetView.reset();
    }
    
    m_tilesetView = std::make_unique<sgc_view::TilesetView>(GetHwnd(), tileWidth, tileHeight);
    m_tilesetView->LoadTileset(path.wstring());
}

void sections::TilesetSection::ClearTileset()
{
    m_tilesetView.reset();
}

void sections::TilesetSection::HandleSectionResize()
{
    if (m_tilesetView != nullptr) {
        RECT rect;
        GetClientRect(GetHwnd(), &rect);
        m_tilesetView->SetScreenSize(rect.right - rect.left, rect.bottom - rect.top);        
    }
    Update();    
}
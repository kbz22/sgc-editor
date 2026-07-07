#include "win32_section/tileset_section.hpp"

win32_section::TilesetSection::TilesetSection(win32_program::Win32Context& context) :
    Section(L"TilesetView", win32_program::ControlId::TilesetView, context),
    m_tilesetView(nullptr)
{}

void win32_section::TilesetSection::Update()
{
    if (m_tilesetView != nullptr) {
        m_tilesetView->Render();
    }
}

void win32_section::TilesetSection::LoadTileset(const std::filesystem::path& path, int tileWidth, int tileHeight)
{   
    if(m_tilesetView != nullptr) {
        m_tilesetView.reset();
    }
    
    m_tilesetView = std::make_unique<sgc_view::TilesetView>(GetHwnd(), tileWidth, tileHeight);
    m_tilesetView->LoadTileset(path.wstring());
}

void win32_section::TilesetSection::ClearTileset()
{
    m_tilesetView.reset();
}

void win32_section::TilesetSection::HandleSectionResize()
{
    if (m_tilesetView != nullptr) {
        RECT rect;
        GetClientRect(GetHwnd(), &rect);
        m_tilesetView->SetScreenSize(rect.right - rect.left, rect.bottom - rect.top);        
    }
    Update();
}
#include "sgc_view/sgc_view.hpp"
#include <sgc/types.hpp>
#include <sgc/sdl/sdl_win32.hpp>
#include <commctrl.h>

namespace {
    constexpr UINT_PTR kTilesetSubclassId = 0x53474331;
}

sgc_view::SgcView::SgcView(HWND hwnd)
    : m_hostWindow(hwnd)
{
    CreateEmbeddedRenderer();
}

sgc_view::SgcView::~SgcView()
{
    if (m_sectionWindow != nullptr) {
        RemoveWindowSubclass(m_sectionWindow, StaticPaneProc, kTilesetSubclassId);
    }

    if (m_renderer != nullptr) {
        sdl::DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }

    if (m_sdlWindow != nullptr) {
        sdl::DestroyWindow(m_sdlWindow);
        m_sdlWindow = nullptr;
    }
}

bool sgc_view::SgcView::CreateEmbeddedRenderer()
{
    if (!m_sdlInitialized && !sdl::InitVideo()) {
        return false;
    }
   
    m_sdlInitialized = true;

    if(m_hostWindow == nullptr) {
        return false;
    }

    m_sdlWindow = sdl::CreateWindowWin32(m_hostWindow, 32, 32);

    if (m_sdlWindow == nullptr) {
        return false;
    }

    m_renderer = sdl::CreateRenderer(m_sdlWindow);
    if (m_renderer == nullptr) {
        sdl::DestroyWindow(m_sdlWindow);
        m_sdlWindow = nullptr;
        return false;
    }

    InstallInputSubclass();

    return true;
}

void sgc_view::SgcView::InstallInputSubclass() {
    if (m_sdlWindow == nullptr) {
        return;
    }

    m_sectionWindow = m_hostWindow;

    auto native = sdl::GetWin32HWND(m_sdlWindow);

    m_sectionWindow = native;
    if (m_sectionWindow != nullptr) {
        SetWindowSubclass(m_sectionWindow, StaticPaneProc, kTilesetSubclassId, reinterpret_cast<DWORD_PTR>(this));
    }
}

LRESULT sgc_view::SgcView::HandleMessages([[maybe_unused]] HWND hwnd, [[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wparam, [[maybe_unused]] LPARAM lparam)
{
    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

LRESULT CALLBACK sgc_view::SgcView::StaticPaneProc([[maybe_unused]] HWND hwnd, [[maybe_unused]] UINT msg, [[maybe_unused]] WPARAM wparam, [[maybe_unused]] LPARAM lparam, [[maybe_unused]] UINT_PTR id, [[maybe_unused]] DWORD_PTR data) 
{
    auto* self = reinterpret_cast<SgcView*>(data);

        if (self)
        {
            return self->HandleMessages(
                hwnd,
                msg,
                wparam,
                lparam);
        }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

bool sgc_view::SgcView::LoadTileset(const std::wstring& path, int tileWidth, int tileHeight)
{
    if (m_renderer == nullptr) {
        return false;
    }

    m_tileWidth  = tileWidth  > 0 ? tileWidth  : defaults::tileSize;
    m_tileHeight = tileHeight > 0 ? tileHeight : defaults::tileSize;

    graphics::Image image{};

    if (!image.LoadTexture(m_renderer, path)) {
        return false;
    }

    const math::uvec2 imageSize = image.GetSize();
    if (imageSize.x == 0 || imageSize.y == 0) {
        return false;
    }

    auto tileVec2 = math::uvec2(m_tileWidth, m_tileHeight);

    m_tileset = std::make_shared<graphics::Tileset>(image, tileVec2); 
    
    return true;
}

void sgc_view::SgcView::DrawAll()
{
    if (m_renderer == nullptr) {
        return;
    }

    sdl::Clear(m_renderer, graphics::color{ 28, 28, 28, 255 });

    if (m_layer != nullptr) {
        m_layer->Draw();
    }  
}

void sgc_view::SgcView::Render()
{
    DrawAll();

    sdl::Render(m_renderer); 
}

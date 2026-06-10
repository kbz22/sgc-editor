#include "sgc/sgc_view.hpp"
#include <sgc/types.hpp>
#include <sgc/sdl/sdl_win32.hpp>
#include <commctrl.h>

namespace {
    constexpr UINT_PTR kTilesetSubclassId = 0x53474331;
}

sgc::SgcView::SgcView(HWND hwnd)
    : m_hostWindow(hwnd)
{
    CreateEmbeddedRenderer();
}

bool sgc::SgcView::CreateEmbeddedRenderer()
{
    if (!sdl::InitVideo()) {
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

void sgc::SgcView::InstallInputSubclass() {
    if (m_sdlWindow == nullptr) {
        return;
    }

    auto native = sdl::GetWin32HWND(m_sdlWindow);

    m_sectionWindow = native;
    if (m_sectionWindow != nullptr) {
        SetWindowSubclass(m_sectionWindow, StaticPaneProc, 0, reinterpret_cast<DWORD_PTR>(this));
    }
}

LRESULT CALLBACK sgc::SgcView::StaticPaneProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR id, DWORD_PTR data) {
    
    switch (msg) {
    case WM_LBUTTONDOWN:

        return 0;

    case WM_MOUSEMOVE:

        break;

    case WM_LBUTTONUP:

        break;

    case WM_CAPTURECHANGED:
        
        break;

    default:
        break;

    }

    return DefSubclassProc(hwnd, msg, wparam, lparam);
}

bool sgc::SgcView::LoadTileset(const std::string& path, int tileWidth, int tileHeight)
{
    if(m_renderer == nullptr) {
        return false;
    }

    m_tileWidth = tileWidth > 0 ? tileWidth : defaults::tileSize;
    m_tileHeight = tileHeight > 0 ? tileHeight : defaults::tileSize;

    image::Image image(m_renderer, path);
    const types::uvec2 imageSize = image.GetSize();
    if (imageSize.x == 0 || imageSize.y == 0) {
        return false;
    }

    const types::unsignedint_t gridWidth = imageSize.x / m_tileWidth;
    const types::unsignedint_t gridHeight = imageSize.y / m_tileHeight;
    
    std::vector<types::uvec2> tilePositions;
    tilePositions.reserve(static_cast<size_t>(gridWidth) * static_cast<size_t>(gridHeight));

    for (types::unsignedint_t row = 0; row < gridHeight; ++row) {
        for (types::unsignedint_t col = 0; col < gridWidth; ++col) {
            tilePositions.push_back({ col, row });
        }
    }

    auto tileVec2 = types::uvec2(m_tileWidth, m_tileHeight);
    m_tileset = std::make_shared<image::Tileset>(std::move(image), tileVec2);

    m_layer = std::make_unique<image::TiledStaticLayer>(
        m_tileset,
        std::move(tilePositions),
        types::uvec2{ gridWidth, gridHeight },
        types::vec2{ 0, 0 }
    );

    Render();

    return true;
}

void sgc::SgcView::Render()
{
    if (m_renderer == nullptr) {
        return;
    }

    sdl::Clear(m_renderer, types::color_t{ 28, 28, 28, 255 });

    if (m_layer != nullptr) {
        m_layer->Draw();
    }    

    sdl::Render(m_renderer); 
}

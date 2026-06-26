#include "sgc_view/sgc_view.hpp"
#include <sgc/types.hpp>
#include <sgc/graphics/viewport.hpp>
#include <sgc/sdl/sdl_win32.hpp>
#include <commctrl.h>
#include "program/except.hpp"
namespace {
    constexpr UINT_PTR kTilesetSubclassId = 0x53474331;
}

sgc_view::SgcView::SgcView(HWND hwnd, int tileWidth, int tileHeight)
    : m_hostWindow(hwnd), m_tileWidth(tileWidth), m_tileHeight(tileHeight)
{
    CreateEmbeddedRenderer();
}

sgc_view::SgcView::~SgcView()
{
    if (m_sectionWindow != nullptr) {
        RemoveWindowSubclass(m_sectionWindow, StaticPaneProc, kTilesetSubclassId);
    }

    if (m_renderContext.renderer != nullptr) {
        sdl::DestroyRenderer(m_renderContext.renderer);
        m_renderContext.renderer = nullptr;
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

    m_renderContext.renderer = sdl::CreateRenderer(m_sdlWindow);
    if (m_renderContext.renderer == nullptr) {
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

bool sgc_view::SgcView::LoadTileset(const std::wstring& path)
{
    if (m_renderContext.renderer == nullptr) {
        return false;
    }

    if(m_tileWidth <= 0 || m_tileHeight <= 0) {
        throw program::TileSizeException("Tile size must be greater than zero.");
    }

    graphics::Image image{};

    if (!image.LoadTexture(m_renderContext.renderer, path)) {
        throw program::AssetLoadException("Failed to load tileset image: " + std::string(path.begin(), path.end()));
    }

    const math::uvec2 imageSize = image.GetSize();
    if (imageSize.x == 0 || imageSize.y == 0) {
        throw program::AssetLoadException("Tileset image has invalid dimensions: " + std::string(path.begin(), path.end()));
    }

    if (imageSize.x % m_tileWidth != 0 || imageSize.y % m_tileHeight != 0) {
        throw program::TileSizeException("Tileset dimensions are not divisible by tile size.");
    }

    if (imageSize.x < m_tileWidth || imageSize.y < m_tileHeight) {
        throw program::TileSizeException("Image is smaller than the specified tile size.");
    }    

    auto tileVec2 = math::uvec2(m_tileWidth, m_tileHeight);

    m_tileset = std::make_shared<graphics::Tileset>(image, tileVec2); 
    
    return true;
}

void sgc_view::SgcView::DrawAll()
{
    if (m_tiledImage != nullptr) {
        m_tiledImage->Draw(this->m_renderContext);
    }  
}

void sgc_view::SgcView::Render()
{
    DrawAll();

    sdl::Render(m_renderContext); 
}

void sgc_view::SgcView::Clear()
{
    sdl::Clear(m_renderContext, graphics::color{ 28, 28, 28, 255 });    
}

void sgc_view::SgcView::SetScreenSize(int width, int height)
{
    m_renderContext.view.screen = graphics::Viewport{ 0, 0, static_cast<float>(width), static_cast<float>(height) };
    m_renderContext.view.camera = m_renderContext.view.screen;
}
#include "sgc_view/sgc_view.hpp"
#include <sgc/math/vector.hpp>
#include <sgc/graphics/viewport.hpp>
#include <sgc/sdl/sdl_win32.hpp>
#include <commctrl.h>
#include "program/except.hpp"

sgc_view::SgcView::SgcView(HWND hwnd, int tileWidth, int tileHeight)
    : m_hostWindow(hwnd), m_tileWidth(tileWidth), m_tileHeight(tileHeight)
{
    CreateEmbeddedRenderer();
}

sgc_view::SgcView::~SgcView()
{
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

    return true;
}

std::shared_ptr<sgc::graphics::Tileset> sgc_view::SgcView::LoadTileset(const std::filesystem::path& path)
{
    if (m_renderContext.renderer == nullptr) {
        return nullptr;
    }

    if(m_tileWidth <= 0 || m_tileHeight <= 0) {
        throw program::TileSizeException("Tile size must be greater than zero.");
    }
        
    std::shared_ptr<graphics::Image> image = std::make_shared<graphics::Image>();

    if (!image->LoadTexture(m_renderContext.renderer, path)) {
        throw program::AssetLoadException("Failed to load tileset image: " + path.string());
    }

    const math::uvec2 imageSize = image->GetSize();
    if (imageSize.x == 0 || imageSize.y == 0) {
        throw program::AssetLoadException("Tileset image has invalid dimensions: " + path.string());
    }

    if (imageSize.x % m_tileWidth != 0 || imageSize.y % m_tileHeight != 0) {
        throw program::TileSizeException("Tileset dimensions are not divisible by tile size.");
    }

    if (imageSize.x < m_tileWidth || imageSize.y < m_tileHeight) {
        throw program::TileSizeException("Image is smaller than the specified tile size.");
    }    

    auto tileVec2 = graphics::PixelSize2D(m_tileWidth, m_tileHeight);

    auto tileset = std::make_shared<graphics::Tileset>(image, tileVec2); 
    
    return tileset;
}

void sgc_view::SgcView::DrawAll()
{
    if (m_drawableImage != nullptr) {
        m_drawableImage->Draw(this->m_renderContext);
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

sgc::math::uvec2 sgc_view::SgcView::PixelsToTiles(sgc::math::uvec2 value) const
{
    return {
        static_cast<sgc::math::uval>(value.x / m_tileWidth),
        static_cast<sgc::math::uval>(value.y / m_tileHeight)
    };
}

sgc::math::vec2 sgc_view::SgcView::PixelsToTiles(sgc::math::vec2 value) const
{
    return {
        static_cast<sgc::math::ival>(value.x / m_tileWidth),
        static_cast<sgc::math::ival>(value.y / m_tileHeight)
    };
}

sgc::graphics::PixelSize2D sgc_view::SgcView::GetTileSize() const
{
    return {
        static_cast<sgc::math::uval>(m_tileWidth),
        static_cast<sgc::math::uval>(m_tileHeight)
    };
}

std::shared_ptr<sgc::graphics::Tileset> sgc_view::SgcView::GetTileset() const
{
    return m_tileset;
}

SDL_Window* sgc_view::SgcView::GetSdlWindow() const
{
    return m_sdlWindow;
}
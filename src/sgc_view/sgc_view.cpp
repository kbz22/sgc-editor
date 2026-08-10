#include "sgc_view/sgc_view.hpp"
#include <sgc/math/vector.hpp>
#include <sgc/graphics/viewport.hpp>
#include <sgc/sdl/sdl_win32.hpp>
#include <commctrl.h>
#include "program/except.hpp"
#include "program/program.hpp"

sgc_view::SgcView::SgcView(HWND hwnd)
{    
    CreateEmbeddedWindow(hwnd);
    Render();
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

bool sgc_view::SgcView::CreateEmbeddedWindow(HWND hostWindow)
{
    if (!m_sdlInitialized && !sdl::InitVideo()) {
        return false;
    }
   
    m_sdlInitialized = true;

    if(hostWindow == nullptr) {
        return false;
    }

    m_sdlWindow = sdl::CreateWindowWin32(hostWindow, 32, 32);

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

void sgc_view::SgcView::SetRenderer(const graphics::RenderContext& context)
{
    m_renderContext.renderer = context.renderer;
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
    sdl::Clear(m_renderContext, m_backgroundColor);    
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

sgc::math::fvec2 sgc_view::SgcView::PixelsToTiles(sgc::math::fvec2 value) const
{
    return {
        value.x / static_cast<float>(m_tileWidth),
        value.y / static_cast<float>(m_tileHeight)
    };
}

sgc::graphics::PixelSize2D sgc_view::SgcView::GetTileSize() const
{
    return {
        static_cast<sgc::math::ival>(m_tileWidth),
        static_cast<sgc::math::ival>(m_tileHeight)
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

sgc::graphics::RenderContext& sgc_view::SgcView::GetRenderContext()
{
    return m_renderContext;
}

void sgc_view::SgcView::SetTileset(sgc::data::AssetId tilesetId)
{
    auto &programContext = program::GetProgramContext();

    std::shared_ptr<sgc::graphics::Tileset> tileset = nullptr;

    try {
        tileset = programContext.assetManager->MakeTileset(tilesetId, &m_renderContext);
    }
    catch ([[maybe_unused]] const program::AssetCacheException& e) {
        // Ignore missing tileset - likely a map was loaded first
        // wait for refresh when the tileset is available        
        return;
    }    

    if(tileset == nullptr) return;
    
    m_tileset = tileset;
    m_tilesetId = tilesetId;
    auto tileSize = tileset->GetTileSize();
    m_tileWidth = static_cast<int>(tileSize.x);
    m_tileHeight = static_cast<int>(tileSize.y);
}

void sgc_view::SgcView::Refresh(program::ProgramContext& programContext)
{
    auto mapDocument = programContext.fileManager->GetActiveDocument();

    if(mapDocument == nullptr) {
        m_backgroundColor = m_backgroundColorInactive;
        return;
    }
    else {
        m_backgroundColor = m_backgroundColorActive;
    }

    auto currentTilesetId = mapDocument->GetTilesetAssetId();

    if(currentTilesetId != m_tilesetId) {
        SetTileset(currentTilesetId);
    }
}
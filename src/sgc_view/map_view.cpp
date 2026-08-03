#include "sgc_view/map_view.hpp"
#include "program/program.hpp"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdexcept>

#include <sgc/graphics/renderlayer.hpp>

sgc_view::MapView::MapView(HWND hwnd) :
    SgcView(hwnd),
    m_cursorTile{nullptr}
{
    std::vector<std::shared_ptr<graphics::IDrawable>> drawables;    

    auto layer = std::make_shared<graphics::RenderLayer>(drawables);

    m_drawableImage = layer;
}

sgc_view::MapView::~MapView()
{    
}

void sgc_view::MapView::SetScreenSize(int width, int height)
{
    auto camera_x = m_renderContext.view.camera.x;
    auto camera_y = m_renderContext.view.camera.y;

    m_renderContext.view.screen = graphics::Viewport{ 0, 0, static_cast<float>(width), static_cast<float>(height) };
    m_renderContext.view.camera = graphics::Viewport{
        camera_x,
        camera_y,
        static_cast<float>(width) * m_zoom,
        static_cast<float>(height) * m_zoom
    };
}

void sgc_view::MapView::SetZoom(float zoom)
{
    m_zoom = zoom;

    auto camera_x = m_renderContext.view.camera.x;
    auto camera_y = m_renderContext.view.camera.y;

    m_renderContext.view.camera = graphics::Viewport{
        camera_x,
        camera_y,
        static_cast<float>(m_renderContext.view.screen.w) * m_zoom,
        static_cast<float>(m_renderContext.view.screen.h) * m_zoom
    };
}

float sgc_view::MapView::GetZoom() const
{
    return m_zoom;
}

void sgc_view::MapView::SetCursorTile(sgc::graphics::PixelSize2D size)
{
    if(m_cursorTile == nullptr) {
        m_cursorTile = std::make_unique<graphics::Rectangle>(0, 0, 0, 0);
    }

    m_cursorTile->SetSize({
        size.x,
        size.y
    });
    m_cursorTile->SetColor({ 255, 255, 255, 64 });
}

void sgc_view::MapView::ResetCursorTile()
{
    m_cursorTile.reset();
}

void sgc_view::MapView::SetTileset(sgc::data::AssetId tilesetId)
{
    SgcView::SetTileset(tilesetId);

    if(m_tileset == nullptr) {
        return;
    }

    auto tileSize = m_tileset->GetTileSize();

    SetCursorTile({
        static_cast<sgc::math::ival>(tileSize.x),
        static_cast<sgc::math::ival>(tileSize.y)
    });   
}

void sgc_view::MapView::ResetTileset()
{
    m_tileset.reset();
    m_drawableImage.reset();
    m_tileWidth = 0;
    m_tileHeight = 0;

    ResetCursorTile();

    Render();
}

void sgc_view::MapView::Render()
{
    SgcView::Clear();
    SgcView::DrawAll();

    if(m_cursorTile != nullptr) {
        m_cursorTile->Draw(m_renderContext);
    }

    sdl::Render(m_renderContext);
}

void sgc_view::MapView::Refresh(program::ProgramContext& programContext)
{
    sgc_view::SgcView::Refresh(programContext);

    auto selectedDocument = programContext.fileManager->GetSelectedDocument();

    if(selectedDocument == nullptr && m_tileset != nullptr) {
        ResetTileset();
        return;
    }

    if(selectedDocument != nullptr && m_tileset == nullptr) {
        auto tilesetId = selectedDocument->GetTilesetAssetId();
        SetTileset(tilesetId);
    }

    if(m_tileset == nullptr) {
        return;
    }

    if(m_cursorTile == nullptr){
        SetCursorTile({
            static_cast<sgc::math::ival>(m_tileWidth),
            static_cast<sgc::math::ival>(m_tileHeight)
        });
    }

    auto layerManager = selectedDocument->GetLayerManager();
    auto layers = layerManager->GetLayers();
    std::vector<std::shared_ptr<graphics::IDrawable>> drawables;

    if(!layers.empty() && layerManager->IsSingleLayerMode()){       

        auto activeLayerIndex = layerManager->GetActiveLayerIndex();
        auto layer = layers[activeLayerIndex];
        layers.clear();

        auto tiledLayer = std::make_shared<graphics::TiledLayer>(
            m_tileset,
            layer.storage
        );

        auto tiledImage = std::make_shared<graphics::TiledImage>(tiledLayer);

        drawables.push_back(tiledImage);
    }

    for(auto layer = layers.rbegin(); layer != layers.rend(); ++layer) {

        if(layer->visible == false) {
            continue;
        }

        auto tiledLayer = std::make_shared<graphics::TiledLayer>(
            m_tileset,
            layer->storage
        );

        auto tiledImage = std::make_shared<graphics::TiledImage>(tiledLayer);

        tiledImage->SetAlpha(layer->transparency);

        drawables.push_back(tiledImage);
    }

    auto layer = std::make_shared<graphics::RenderLayer>(drawables);

    m_drawableImage = layer;

    Render();
}

void sgc_view::MapView::SetCursorPositionInPixels(sgc::graphics::PixelPosition2D position)
{
    m_cursorTile->SetPosition({
        position.x,
        position.y
    });
}

void sgc_view::MapView::SetCursorSizeInPixels(sgc::graphics::PixelSize2D size)
{
    m_cursorTile->SetSize({
        size.x,
        size.y
    });
}

sgc::graphics::PixelPosition2D sgc_view::MapView::GetCursorPositionInPixels() const
{
    return m_cursorTile->GetPosition();
}

sgc::tile::TilePosition2D sgc_view::MapView::GetCursorPositionInTiles() const
{
    auto pixelPosition = m_cursorTile->GetPosition();
    
    return {
        pixelPosition.x / static_cast<sgc::math::ival>(m_tileWidth),
        pixelPosition.y / static_cast<sgc::math::ival>(m_tileHeight)
    };
}

sgc::graphics::PixelSize2D sgc_view::MapView::GetCursorSizeInPixels() const
{
    return m_cursorTile->GetSize();
}

sgc::tile::TileSize2D sgc_view::MapView::GetCursorSizeInTiles() const
{
    auto pixelSize = m_cursorTile->GetSize();
    
    return {
        pixelSize.x / m_tileWidth,
        pixelSize.y / m_tileHeight
    };
}

void sgc_view::MapView::SetCameraPositionSingles(float x, float y)
{
    m_renderContext.view.camera.x = x;
    m_renderContext.view.camera.y = y;
}

void sgc_view::MapView::ChangeCameraPositionSingles(float deltaX, float deltaY)
{
    m_renderContext.view.camera.x -= deltaX;
    m_renderContext.view.camera.y -= deltaY;
}

void sgc_view::MapView::ChangeCursorPositionInPixels(sgc::graphics::PixelPosition2D delta)
{
    auto currentPosition = m_cursorTile->GetPosition();
    
    m_cursorTile->SetPosition({
        currentPosition.x - delta.x,
        currentPosition.y - delta.y
    });
}

sgc::math::fvec2 sgc_view::MapView::GetCameraPositionSingles() const
{
    return {
        m_renderContext.view.camera.x,
        m_renderContext.view.camera.y
    };
}

sgc::graphics::View sgc_view::MapView::GetView() const
{
    return m_renderContext.view;
}
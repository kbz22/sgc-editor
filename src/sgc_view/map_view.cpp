#include "sgc_view/map_view.hpp"
#include "program/program.hpp"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdexcept>

#include <sgc/graphics/renderlayer.hpp>

sgc_view::MapView::MapView(HWND hwnd) :
    SgcView(hwnd),
    m_cursorTile{0, 0, 0, 0}
{    
    m_cursorTile.SetColor({ 255, 255, 255, 64 });

    std::vector<std::shared_ptr<graphics::IDrawable>> drawables;    

    auto layer = std::make_shared<graphics::RenderLayer>(drawables);

    m_drawableImage = layer;
}

sgc_view::MapView::~MapView()
{    
}

void sgc_view::MapView::SetTileset(std::shared_ptr<graphics::Tileset> tileset)
{
    SgcView::SetTileset(tileset);

    auto tileSize = m_tileset->GetTileSize();

    m_cursorTile.SetSize({
        static_cast<sgc::math::ival>(tileSize.x),
        static_cast<sgc::math::ival>(tileSize.y)
    });    
}

void sgc_view::MapView::Render()
{
    SgcView::Clear();
    SgcView::DrawAll();

    m_cursorTile.Draw(m_renderContext);

    sdl::Render(m_renderContext);
}

void sgc_view::MapView::Refresh(program::LayerManager& layerManager)
{
    auto layers = layerManager.GetLayers();
    std::vector<std::shared_ptr<graphics::IDrawable>> drawables;

    if(!layers.empty() && layerManager.IsSingleLayerMode()){       

        auto activeLayerIndex = layerManager.GetActiveLayerIndex();
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
    m_cursorTile.SetPosition({
        position.x,
        position.y
    });
}

void sgc_view::MapView::SetCursorSizeInPixels(sgc::graphics::PixelSize2D size)
{
    m_cursorTile.SetSize({
        size.x,
        size.y
    });
}

sgc::graphics::PixelPosition2D sgc_view::MapView::GetCursorPositionInPixels() const
{
    return m_cursorTile.GetPosition();
}

sgc::tile::TilePosition2D sgc_view::MapView::GetCursorPositionInTiles() const
{
    auto pixelPosition = m_cursorTile.GetPosition();
    
    return {
        pixelPosition.x / static_cast<sgc::math::ival>(m_tileWidth),
        pixelPosition.y / static_cast<sgc::math::ival>(m_tileHeight)
    };
}

sgc::graphics::PixelSize2D sgc_view::MapView::GetCursorSizeInPixels() const
{
    return m_cursorTile.GetSize();
}

sgc::tile::TileSize2D sgc_view::MapView::GetCursorSizeInTiles() const
{
    auto pixelSize = m_cursorTile.GetSize();
    
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
    auto currentPosition = m_cursorTile.GetPosition();
    m_cursorTile.SetPosition({
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
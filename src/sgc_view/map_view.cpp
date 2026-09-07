#include "sgc_view/map_view.hpp"
#include "program/program.hpp"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdexcept>
#include <chrono>

#include <sgc/graphics/drawablecontainer.hpp>

sgc_view::MapView::MapView(HWND hwnd) :
    SgcView(hwnd),
    m_cursorTile{nullptr}
{
    m_tileGrid = std::make_shared<graphics::LineGrid>(
        0, 0,
        0, 0,
        static_cast<int>(sgc::data::TileChunk::Size),
        static_cast<int>(sgc::data::TileChunk::Size)
    );
    m_tileGrid->SetColor({ 128, 128, 128, 64 });

    m_chunkGrid = std::make_shared<graphics::LineGrid>(
        0, 0,
        0, 0,
        static_cast<int>(sgc::data::TileChunk::Size * sgc::data::TileChunk::Size),
        static_cast<int>(sgc::data::TileChunk::Size * sgc::data::TileChunk::Size)
    );
    m_chunkGrid->SetColor({ 255, 255, 255, 255 });

    m_marchingAntsRectangleOnMap = std::make_shared<graphics::MarchingAntsRectangle>(
        sgc::math::fvec2{0.0f, 0.0f},
        sgc::math::fvec2{0.0f, 0.0f}
    );

    m_mapLayers = std::make_shared<graphics::DrawableLayers>();
    m_selectionLayers = std::make_shared<graphics::DrawableLayers>();
    m_drawableImage = m_mapLayers;
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
        ScaleForZoom(static_cast<float>(width)),
        ScaleForZoom(static_cast<float>(height))
    };
}

bool sgc_view::MapView::SetZoom(float zoom)
{
    if(zoom == m_zoom)
    {
        return false;
    }

    m_zoom = zoom;

    auto camera_x = m_renderContext.view.camera.x;
    auto camera_y = m_renderContext.view.camera.y;

    m_renderContext.view.camera = graphics::Viewport{
        camera_x,
        camera_y,
        ScaleForZoom(static_cast<float>(m_renderContext.view.screen.w)),
        ScaleForZoom(static_cast<float>(m_renderContext.view.screen.h))
    };
    
    return true;
}

float sgc_view::MapView::GetZoom() const
{
    return m_zoom;
}

void sgc_view::MapView::UpdateGridPosition(sgc::graphics::Viewport cameraViewport)
{
    using sgc::math::ival;

    auto tileSize = m_tileset->GetTileSize();
    auto chunkOffsetX = static_cast<ival>(cameraViewport.x) % static_cast<ival>(tileSize.x * sgc::data::TileChunk::Size) + sgc::data::TileChunk::Size * tileSize.x;
    auto chunkOffsetY = static_cast<ival>(cameraViewport.y) % static_cast<ival>(tileSize.y * sgc::data::TileChunk::Size) + sgc::data::TileChunk::Size * tileSize.y;

    auto gridPosition = sgc::graphics::PixelPosition2D{
        static_cast<ival>(cameraViewport.x) - chunkOffsetX,
        static_cast<ival>(cameraViewport.y) - chunkOffsetY
    };
    auto gridSize = sgc::graphics::PixelSize2D{
        static_cast<ival>(cameraViewport.w) + tileSize.x + chunkOffsetX,
        static_cast<ival>(cameraViewport.h) + tileSize.y + chunkOffsetY
    };

    m_tileGrid->SetPosition(gridPosition);
    m_tileGrid->SetSize(gridSize);

    m_chunkGrid->SetPosition(gridPosition);
    m_chunkGrid->SetSize(gridSize);
}

void sgc_view::MapView::UpdateSelectionOffset()
{
    auto segmentLength = m_marchingAntsRectangleOnMap->GetSegmentLength();
    auto sinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - m_lastSelectionOffsetUpdateTime
    ).count();

    m_offset += static_cast<float>(sinceLastUpdate) / 100.0f;

    if(m_offset > 0.0f){
        m_lastSelectionOffsetUpdateTime = std::chrono::steady_clock::now();
        if(m_offset > segmentLength * 2.0f) {
            m_offset = 0.0f;
        }
    }

    m_marchingAntsRectangleOnMap->SetOffset(
        m_offset
    );
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
    // SgcView::DrawAll();

    for(auto [layerIndex, layer] : *m_mapLayers) {
        auto selectionLayer = m_selectionLayers->Get(layerIndex);

        if(layer != nullptr) {
            layer->Draw(m_renderContext);
        }
        if(selectionLayer != nullptr) {
            selectionLayer->Draw(m_renderContext);
        }
    }

    auto selectBoxSize = m_marchingAntsRectangleOnMap->GetSize();
    if(selectBoxSize.x > 0.0f && selectBoxSize.y > 0.0f) {
        m_marchingAntsRectangleOnMap->Draw(m_renderContext);
    }

    if(m_cursorTile != nullptr) {
        m_cursorTile->Draw(m_renderContext);
    }

    sdl::Render(m_renderContext);
}

void sgc_view::MapView::RenderToImage(std::filesystem::path outputPath)
{
    sdl::RenderAsImage(
        outputPath,
        m_renderContext,
        [this]([[maybe_unused]] graphics::RenderContext const& context) {
            SgcView::Clear();

            for(auto [layerIndex, layer] : *m_mapLayers) {
                if(layer != nullptr) {
                    layer->Draw(m_renderContext);
                }
            }

            sdl::Render(m_renderContext);
        }
    );
}

void sgc_view::MapView::Refresh(program::ProgramContext& programContext)
{
    sgc_view::SgcView::Refresh(programContext);
    
    m_mapLayers->Clear();

    auto selectedDocument = programContext.fileManager->GetActiveDocument();

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
    size_t layerIndex = 0;    

    if(!layers.empty() && layerManager->IsSingleLayerMode()){       

        auto activeLayerIndex = layerManager->GetActiveLayerIndex();
        auto layer = layers[activeLayerIndex];
        layers.clear();

        auto tiledLayer = std::make_shared<graphics::TiledLayer>(
            m_tileset,
            layer.storage
        );

        auto tiledImage = std::make_shared<graphics::TiledImage>(tiledLayer);
        
        m_mapLayers->Set(layerIndex, tiledImage);
        layerIndex++;
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

        m_mapLayers->Set(layerIndex, tiledImage);
        layerIndex++;
    }

    if(program::HasFlag(programContext.editorGridMode, program::EditorGridMode::TileGrid)) {
        m_mapLayers->Set(layerIndex, m_tileGrid);
        layerIndex++;        
    }
    
    if(program::HasFlag(programContext.editorGridMode, program::EditorGridMode::ChunkGrid)) {
        m_mapLayers->Set(layerIndex, m_chunkGrid);
        layerIndex++;        
    }

    UpdateGridPosition(m_renderContext.view.camera);

    Render();
}

void sgc_view::MapView::SetCursorPositionInPixels(sgc::graphics::PixelPosition2D position)
{
    m_cursorTile->SetPosition({
        position.x,
        position.y
    });

    m_onCursorPositionChangedCallback({
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

void sgc_view::MapView::SetSelectionPositionInPixels(sgc::graphics::PixelPosition2D position)
{
    m_marchingAntsRectangleOnMap->SetPosition({
        static_cast<float>(position.x),
        static_cast<float>(position.y)
    });
}

void sgc_view::MapView::SetSelectionSizeInPixels(sgc::graphics::PixelSize2D size)
{
    m_marchingAntsRectangleOnMap->SetSize({
        static_cast<float>(size.x),
        static_cast<float>(size.y)
    });

    m_onSelectionSizeChangedCallback(size);
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

sgc::tile::TilePosition2D sgc_view::MapView::GetSelectionPositionInTiles() const
{
    auto pixelPosition = m_marchingAntsRectangleOnMap->GetPosition();
    
    return {
        static_cast<sgc::math::ival>(pixelPosition.x) / m_tileWidth,
        static_cast<sgc::math::ival>(pixelPosition.y) / m_tileHeight
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

sgc::tile::TileSize2D sgc_view::MapView::GetSelectionSizeInTiles() const
{
    auto pixelSize = m_marchingAntsRectangleOnMap->GetSize();
    
    return {
        static_cast<sgc::math::ival>(pixelSize.x) / m_tileWidth,
        static_cast<sgc::math::ival>(pixelSize.y) / m_tileHeight
    };
}

void sgc_view::MapView::SetCameraPositionSingles(float x, float y)
{
    m_renderContext.view.camera.x = x;
    m_renderContext.view.camera.y = y;
    UpdateGridPosition(m_renderContext.view.camera);
}

void sgc_view::MapView::ChangeCameraPositionSingles(float deltaX, float deltaY)
{
    m_renderContext.view.camera.x -= deltaX;
    m_renderContext.view.camera.y -= deltaY;
    UpdateGridPosition(m_renderContext.view.camera);
}

void sgc_view::MapView::ChangeCursorPositionInPixels(sgc::graphics::PixelPosition2D delta)
{
    auto currentPosition = m_cursorTile->GetPosition();

    auto newPosition = sgc::graphics::PixelPosition2D{
        currentPosition.x - delta.x,
        currentPosition.y - delta.y
    };
    
    m_cursorTile->SetPosition({
        newPosition.x,
        newPosition.y
    });

    m_onCursorPositionChangedCallback({
        newPosition.x,
        newPosition.y
    });
}

void sgc_view::MapView::RegisterOnCursorPositionChangedCallback(std::function<void(sgc::math::vec2)> callback)
{
    m_onCursorPositionChangedCallback = callback;
}

void sgc_view::MapView::RegisterOnSelectionSizeChangedCallback(std::function<void(sgc::math::vec2)> callback)
{
    m_onSelectionSizeChangedCallback = callback;
}

sgc::math::fvec2 sgc_view::MapView::GetCameraPositionSingles() const
{
    return {
        m_renderContext.view.camera.x,
        m_renderContext.view.camera.y
    };
}

sgc::math::fvec2 sgc_view::MapView::GetCursorPositionSingles() const
{
    auto cursorPosition = m_cursorTile->GetPosition();

    return {
        static_cast<float>(cursorPosition.x),
        static_cast<float>(cursorPosition.y)
    };
}

sgc::graphics::View sgc_view::MapView::GetView() const
{
    return m_renderContext.view;
}

sgc::graphics::DrawableLayers* sgc_view::MapView::GetSelectionLayers() const
{
    return m_selectionLayers.get();
}
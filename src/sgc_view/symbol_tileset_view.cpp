#include "sgc_view/symbol_tileset_view.hpp"
#include "program/program.hpp"
#include <sgc/data/statictilestorage.hpp>

bool IsValid(sgc::math::vec4 area, program::SgcTileset &sgcTileset);

sgc_view::SymbolTilesetView::SymbolTilesetView(HWND hwnd) :
    SgcView{hwnd},
    m_symbolIndicator{std::make_unique<sgc::graphics::Rectangle>(0,0,0,0)},
    m_cursor{std::make_unique<sgc::graphics::Rectangle>(0,0,0,0)}
{}

void sgc_view::SymbolTilesetView::Render()
{
    SgcView::Clear();
    SgcView::DrawAll();

    if(m_symbolIndicator != nullptr) 
    {
        for(auto &position : m_symbolPositions)
        {
           m_symbolIndicator->SetPosition(position);
           m_symbolIndicator->Draw(m_renderContext);
        }        
    }

    if(m_cursor != nullptr && m_cursorVisible)
    {
        m_cursor->Draw(m_renderContext);
    }

    sdl::Render(m_renderContext);
}

void sgc_view::SymbolTilesetView::ResetTileset()
{
    m_sgcTileset.Reset();
    m_layer.reset();
    m_drawableImage.reset();

    Render();
}

void sgc_view::SymbolTilesetView::Refresh(program::ProgramContext& programContext)
{
    sgc_view::SgcView::Refresh(programContext);
    auto selectedDocument = programContext.GetManager<file::FileManager>()->GetActiveDocument();

    if(selectedDocument == nullptr && m_sgcTileset.IsSet()) {
        ResetTileset();
        return;
    }

    if(selectedDocument != nullptr && !m_sgcTileset.IsSet()) {
        auto tilesetId = selectedDocument->GetTilesetAssetId();
        SetTileset(tilesetId);
    }

    if(!m_sgcTileset.IsSet()) {
        return;
    }

    auto currentPositions = GetSymbolPositions();
    auto newSize = GetSymbolSize();
    for(auto &position : currentPositions)
    {
        auto areaVector = sgc::math::vec4{
            position.x,
            position.y,
            position.x+newSize.x,
            position.y+newSize.y
        };

        if(IsValid(areaVector, m_sgcTileset)){
            m_symbolPositions.push_back(position);
        }
    }
}

void sgc_view::SymbolTilesetView::SetTileset(sgc::data::AssetId tilesetId)
{
    SgcView::SetTileset(tilesetId);

    if(!m_sgcTileset.IsSet()) {
        return;
    }

    auto tileset = m_sgcTileset.GetTileset();
    const math::vec2 gridSize = tileset->GetSizeInTiles();
    auto tileStorage = std::make_shared<data::StaticTileStorage>(math::vec2{gridSize.x, gridSize.y});

    for (sgc::tile::TileId i = 0; i < gridSize.y * gridSize.x; ++i) {  
        tileStorage->SetTileAt({
            static_cast<sgc::math::ival>(i % gridSize.x),
            static_cast<sgc::math::ival>(i / gridSize.x)
        }, 
            i
    );
    }

    m_layer = std::make_shared<graphics::TiledLayer>(
        tileset,
        tileStorage
    );

    auto tiledImage = std::make_shared<graphics::TiledImage>(m_layer);    
    tiledImage->SetPositionPixels(sgc::graphics::PixelPosition2D{
        0,
        m_sgcTileset.GetVerticalOffsetInPixels()
    });
    m_drawableImage = tiledImage;
}

bool IsValid(sgc::math::vec4 area, program::SgcTileset &sgcTileset)
{    
    for(auto x = area.x; x<area.z; x++){
        for(auto y = area.y; y<area.w; y++)
        {
            if(!sgcTileset.IsValid({x,y}))
                return false;
        }
    }
    return true;
}

void sgc_view::SymbolTilesetView::AddTilePosition(sgc::tile::TilePosition2D position)
{
    auto tileSize = GetTileSize();
    auto symbolSize = GetSymbolSize();

    bool isValid = IsValid(
        {position.x, position.y, position.x + symbolSize.x, position.y + symbolSize.y},
        m_sgcTileset
    );

    if(isValid)
    {
        m_symbolPositions.push_back({
            position.x * tileSize.x,
            position.y * tileSize.y,
        });
    }
}

void sgc_view::SymbolTilesetView::AddTilePosition(std::vector<sgc::tile::TilePosition2D> &positions)
{    
    for(auto &position : positions)
    {
        AddTilePosition(position);
    }
}

void sgc_view::SymbolTilesetView::SetSizeInTiles(sgc::tile::TileSize2D size)
{
    auto tileSize = GetTileSize();
    m_symbolIndicator->SetSize({
        size.x * tileSize.x,
        size.y * tileSize.y
    });
    m_cursor->SetSize({
        size.x * tileSize.x,
        size.y * tileSize.y
    });

    // remove all invalid positions
    auto positions = GetSymbolPositions();
    ClearSymbolPositions();
    AddTilePosition(positions);
}

void sgc_view::SymbolTilesetView::SetCursorColor(sgc::graphics::color color)
{
    m_cursor->SetColor(color);
}

void sgc_view::SymbolTilesetView::SetIndicatorColor(sgc::graphics::color color)
{
    m_symbolIndicator->SetColor(color);
}


std::vector<sgc::tile::TilePosition2D> sgc_view::SymbolTilesetView::GetSymbolPositions() const
{
    std::vector<sgc::tile::TilePosition2D> returnPositions;
    auto tileSize = GetTileSize();

    for(auto &position : m_symbolPositions)
    {
        returnPositions.push_back({
            position.x / tileSize.x,
            position.y / tileSize.y
        });
    }

    return returnPositions;
}

sgc::tile::TilePosition2D sgc_view::SymbolTilesetView::GetSymbolSize() const
{
    auto tileSize = m_sgcTileset.GetTileSize();    
    auto indicatorSize = m_symbolIndicator->GetSize();
    return {
        indicatorSize.x / tileSize.x,
        indicatorSize.y / tileSize.y
    };
}

sgc::tile::TileSize2D sgc_view::SymbolTilesetView::GetMaxSymbolSize() const
{
    auto tileSize = m_sgcTileset.GetTileSize();    
    auto imageSize = m_sgcTileset.GetImageSize();
    return {
        imageSize.x / tileSize.x,
        imageSize.y / tileSize.y
    };
}

sgc::graphics::PixelPosition2D sgc_view::SymbolTilesetView::GetImageSize() const
{
    return m_layer->GetTileset().GetImageSize();
}

sgc::graphics::PixelPosition2D sgc_view::SymbolTilesetView::GetTilesetImageSize() const
{
    return m_sgcTileset.GetTilesetImageSize();
}

std::vector<sgc::tile::TilePosition2D> sgc_view::SymbolTilesetView::ClearSymbolPositions()
{
    auto symbolPositions = GetSymbolPositions();
    m_symbolPositions.clear();

    return symbolPositions;
}

bool sgc_view::SymbolTilesetView::Empty()
{
    return m_symbolPositions.empty();
}

bool sgc_view::SymbolTilesetView::IsCursorVisible()
{
    return m_cursorVisible;
}

void sgc_view::SymbolTilesetView::ShowCursor(bool value)
{
    m_cursorVisible = value;
}

void sgc_view::SymbolTilesetView::SetCursorPosition(sgc::tile::TilePosition2D position)
{
    auto tileSize = GetTileSize();
    m_cursor->SetPosition({
        position.x * tileSize.x,
        position.y * tileSize.y
    });
}

sgc::tile::TilePosition2D sgc_view::SymbolTilesetView::GetCursorPosition() const
{
    auto tileSize = GetTileSize();
    auto cursorPxPos = m_cursor->GetPosition();
    return {
        cursorPxPos.x / tileSize.x,
        cursorPxPos.y / tileSize.y
    };
}
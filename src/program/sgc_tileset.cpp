#include "program/sgc_tileset.hpp"
#include "program/except.hpp"
#include <algorithm>

bool program::SgcTileset::Set(sgc::data::AssetId tilesetId, file::AssetManager &assetManager, sgc::graphics::RenderContext &renderContext)
{
    try {
        m_tileset = assetManager.MakeTileset(tilesetId, &renderContext);
    }
    catch (const program::AssetCacheException&) {
        // Ignore missing tileset - likely a map was loaded first
        // wait for refresh when the tileset is available        
        return false;
    }    

    if(m_tileset == nullptr) return false;

    return true;
}

void program::SgcTileset::Reset()
{
    m_tileset.reset();
}

void program::SgcTileset::RegisterSpecialTile(const std::wstring &name)
{
    m_paddedTiles.push_back(name);
    // remove duplicates if there are any
    m_paddedTiles.erase(
        std::unique(m_paddedTiles.begin(), m_paddedTiles.end()),
        m_paddedTiles.end()
    );
}

std::shared_ptr<sgc::graphics::Tileset> program::SgcTileset::GetTileset() const
{
    return m_tileset;
}

std::optional<sgc::tile::TileId> program::SgcTileset::GetTileId(sgc::tile::TilePosition2D positionOnTileset)  const
{
    auto sizeInTiles = m_tileset->GetSizeInTiles();
    auto extraRows = static_cast<sgc::math::ival>(m_paddedTiles.size()) / sizeInTiles.x + 1;

    if(positionOnTileset.y < extraRows)
    {
        positionOnTileset.y += sizeInTiles.y;
    }
    else
    {
        positionOnTileset.y -= extraRows;
    }

    if(!IsValid(positionOnTileset)){
        return std::nullopt;
    }

    auto tileId = m_tileset->ToTileId(positionOnTileset);

    if(!IsValid(tileId)){
        return std::nullopt;
    }

    return {tileId};
}

bool program::SgcTileset::IsSpecial(sgc::tile::TileId tileId) const
{
    return tileId >= m_tileset->TileIdCount();
}

bool program::SgcTileset::IsValid(sgc::tile::TileId tileId) const
{
    auto count = m_tileset->TileIdCount() + static_cast<sgc::tile::TileId>(m_paddedTiles.size());

    return tileId < count;
}

bool program::SgcTileset::IsValid(sgc::tile::TilePosition2D positionOnTileset) const
{
    auto sizeInTiles = m_tileset->GetSizeInTiles();
    auto extraRows = static_cast<sgc::math::ival>(m_paddedTiles.size()) / sizeInTiles.x + 1;

    if(positionOnTileset.x >= sizeInTiles.x || positionOnTileset.y >= (sizeInTiles.y+extraRows) || positionOnTileset.x < 0 || positionOnTileset.y < 0)
    {
        return false;
    }

    if(positionOnTileset.y == extraRows - 1 && positionOnTileset.x >= static_cast<sgc::math::ival>(m_paddedTiles.size() % sizeInTiles.x))
    {
        return false;
    }

    return true;
}

std::wstring program::SgcTileset::GetSpecialTileName(sgc::tile::TileId tileId) const
{
    int index = m_tileset->TileIdCount() - tileId;

    if(index < 0 || index >= m_paddedTiles.size())
        return L"no text :(";

    return m_paddedTiles[index];
}

bool program::SgcTileset::IsSet() const
{
    return m_tileset != nullptr;
}

sgc::graphics::PixelCount program::SgcTileset::GetVerticalOffsetInPixels() const
{
    auto extraRows = static_cast<sgc::math::ival>(m_paddedTiles.size()) / m_tileset->GetSizeInTiles().x + 1;
    return extraRows * m_tileset->GetTileSize().y;
}
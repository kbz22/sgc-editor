#include "file/tileset_document.hpp"

file::TilesetDocument::TilesetDocument(sgc::data::AssetId tilesetId) :
    m_tilesetId{tilesetId}
{}

sgc::data::AssetId file::TilesetDocument::GetTilesetAssetId() const
{
    return m_tilesetId;
}
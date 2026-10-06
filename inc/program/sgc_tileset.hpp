#pragma once

#include "file/asset_manager.hpp"
#include <sgc/graphics/tileset.hpp>
#include <sgc/data/asset.hpp>
#include <sgc/math/vector.hpp>
#include <sgc/tile/tile.hpp>
#include <vector>
#include <string>
#include <cstdint>
#include <optional>

namespace program
{
    class SgcTileset
    {
        private:
            std::shared_ptr<sgc::graphics::Tileset> m_tileset{nullptr};            
            std::vector<std::wstring> m_paddedTiles{
                L"Empty"
            };

        public:
            bool Set(sgc::data::AssetId tilesetId, file::AssetManager &assetManager, sgc::graphics::RenderContext &renderContext);
            void Reset();
            void RegisterSpecialTile(const std::wstring &name);

            std::shared_ptr<sgc::graphics::Tileset> GetTileset() const;
            std::optional<sgc::tile::TileId> GetTileId(sgc::tile::TilePosition2D positionOnTileset) const;

            bool IsSet() const;
            bool IsValid(sgc::tile::TileId tileId) const;
            bool IsValid(sgc::tile::TilePosition2D positionOnTileset) const;
            bool IsSpecial(sgc::tile::TileId tileId) const;
            bool IsSpecial(sgc::tile::TilePosition2D positionOnTileset) const;

            std::wstring GetSpecialTileName(sgc::tile::TileId tileId) const;
            sgc::graphics::PixelCount GetVerticalOffsetInPixels() const;            
            size_t GetSpecialTileCount() const;
            sgc::tile::TileSize2D GetSizeInTiles() const;
            sgc::graphics::PixelSize2D GetTileSize() const;
            sgc::tile::TilePosition2D GetTilePosition(sgc::tile::TileId tileId) const;
    };
}
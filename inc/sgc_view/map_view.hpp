#pragma once

#include "sgc_view/sgc_view.hpp"
#include "program/layer_manager.hpp"

#include <sgc/math/vector.hpp>
#include <sgc/data/chunkedtilestorage.hpp>
#include <sgc/graphics/rectangle.hpp>

#include <filesystem>

namespace sections {
    class MapSection;
}

namespace sgc_view
{
    using namespace sgc;

    class MapView : public SgcView
    {
        private:            
            graphics::Rectangle m_cursorTile = graphics::Rectangle{ 0, 0, defaults::tileSize, defaults::tileSize }; 

        public:
            MapView(HWND hwnd, std::filesystem::path tilesetPath, int tileWidth = defaults::tileSize, int tileHeight = defaults::tileSize);
            ~MapView();
            
            void Render() override;
            void Refresh(program::LayerManager& layerManager);

            void SetCursorPositionInPixels(sgc::graphics::PixelPosition2D position);
            void SetCursorSizeInPixels(sgc::graphics::PixelSize2D size);

            sgc::graphics::PixelPosition2D GetCursorPositionInPixels() const;
            sgc::tile::TilePosition2D GetCursorPositionInTiles() const;
            sgc::graphics::PixelSize2D GetCursorSizeInPixels() const;
            sgc::tile::TileSize2D GetCursorSizeInTiles() const;

            friend class sections::MapSection;
    };
}
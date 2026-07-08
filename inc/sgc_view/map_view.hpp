#pragma once

#include "sgc_view/sgc_view.hpp"

#include <sgc/math/vector.hpp>
#include <sgc/data/chunkedtilestorage.hpp>
#include <sgc/graphics/rectangle.hpp>

#include <vector>

namespace sections {
    class MapSection;
}

namespace sgc_view
{
    using namespace sgc;

    class MapView : public SgcView
    {
        private:
            std::shared_ptr<data::ChunkedTileStorage> m_tileStorage = nullptr;
            graphics::Rectangle m_cursorTile = graphics::Rectangle{ 0, 0, defaults::tileSize, defaults::tileSize };            
            bool m_isPainting = false;
            sgc::tile::TilesetPosition2D m_selectionTileStart = {0,0};

            void AddChunk(sgc::data::ChunkCoord chunkCoord);
            void RemoveChunk(sgc::data::ChunkCoord chunkCoord);
            
            sgc::math::uvec2 GetCursorSizeInTiles() const;
            void SetStorage(std::shared_ptr<sgc::data::ChunkedTileStorage> storage);        

        protected:
            LRESULT HandleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;

        public:
            MapView(HWND hwnd, int tileWidth = defaults::tileSize, int tileHeight = defaults::tileSize);
            ~MapView();

            bool LoadTileset(const std::wstring& path) override;
            void Render() override;

            friend class sections::MapSection;
    };
}
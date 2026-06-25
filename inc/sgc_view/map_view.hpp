#pragma once

#include "sgc_view/sgc_view.hpp"

#include <sgc/types.hpp>
#include <sgc/data/chunkedtilestorage.hpp>

namespace sgc_view
{
    using namespace sgc;

    class MapView : public SgcView
    {
        private:
            std::shared_ptr<data::ChunkedTileStorage> m_tileStorage = nullptr;
            tile::TileId m_currentTileId = 0;

            void AddChunk(sgc::data::ChunkCoord chunkCoord);
            void RemoveChunk(sgc::data::ChunkCoord chunkCoord);            

        protected:
            LRESULT HandleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;

        public:
            MapView(HWND hwnd);
            ~MapView();

            void SetTile(int tileX, int tileY);
            void SetTile(tile::TileId tileId);

            bool LoadTileset(const std::wstring& path, int tileWidth = defaults::tileSize, int tileHeight = defaults::tileSize);
    };
}
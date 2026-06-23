#pragma once

#include "sgc_view/sgc_view.hpp"

#include <sgc/types.hpp>
#include <sgc/data/statictilestorage.hpp>

namespace sgc_view
{
    using namespace sgc;

    class MapView : public SgcView
    {
        private:
            std::shared_ptr<data::StaticTileStorage> m_tileStorage = nullptr;

        protected:
            LRESULT HandleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;

        public:
            MapView(HWND hwnd);
            ~MapView();

            void SetTile(int tileX, int tileY);

            bool LoadTileset(const std::wstring& path, int tileWidth = defaults::tileSize, int tileHeight = defaults::tileSize);
    };
}
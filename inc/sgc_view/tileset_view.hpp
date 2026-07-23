#pragma once

#include <sgc_view/sgc_view.hpp>
#include <sgc/graphics/rectangle.hpp>
#include <sgc/graphics/tiledlayer.hpp>
#include "defaults.hpp"

namespace sgc_view 
{
    class TilesetView : public SgcView
    {
        private:
            math::uval m_gridWidth = 0;
            math::uval m_gridHeight = 0;
            std::shared_ptr<graphics::TiledLayer> m_layer = nullptr;

        public:
            TilesetView(HWND hwnd);
            ~TilesetView();

            void Render() override;
            void SetTileset(std::shared_ptr<graphics::Tileset> tileset) override;
    };    
}
#pragma once

#include <sgc/graphics/tileset.hpp>
#include <sgc/graphics/tiledimage.hpp>
#include <sgc/data/asset.hpp>   

#undef CreateWindow // avoid macro name conflict with sdl::CreateWindow

#include <sgc/sdl/sdl.hpp>
#include <windows.h>
#include <filesystem>

#include "defaults.hpp"

namespace program {
    struct ProgramContext;
}

namespace sections {
    class MapSection;
    class TilesetSection;
}

namespace sgc_view
{
    using namespace sgc;

    class SgcView
    {
        private:
            bool m_sdlInitialized = false;

            SDL_Window* m_sdlWindow = nullptr;
            
            bool CreateEmbeddedWindow(HWND hostWindow);

        protected:
            int m_tileWidth;
            int m_tileHeight;
            std::shared_ptr<graphics::Tileset> m_tileset = nullptr;
            sgc::data::AssetId m_tilesetId;
            std::shared_ptr<graphics::IDrawable> m_drawableImage = nullptr;
            graphics::RenderContext m_renderContext{};
            const sgc::graphics::color m_backgroundColorActive{ 28, 28, 28, 255 };
            const sgc::graphics::color m_backgroundColorInactive{ 255, 255, 255, 255 };
            sgc::graphics::color m_backgroundColor{ m_backgroundColorInactive };

            void DrawAll();
            virtual void SetTileset(sgc::data::AssetId tilesetId);
            virtual void SetRenderer(const graphics::RenderContext& context);

        public:
            SgcView(HWND hwnd);
            ~SgcView();
            
            virtual void SetScreenSize(int width, int height);
            virtual void Render();
            virtual void Refresh(program::ProgramContext& programContext);

            graphics::RenderContext& GetRenderContext();

            std::shared_ptr<graphics::Tileset> GetTileset() const;
            sgc::graphics::PixelSize2D GetTileSize() const;
            SDL_Window* GetSdlWindow() const;
            
            sgc::math::uvec2 PixelsToTiles(sgc::math::uvec2 value) const;
            sgc::math::vec2 PixelsToTiles(sgc::math::vec2 value) const;
            sgc::math::fvec2 PixelsToTiles(sgc::math::fvec2 value) const;

            void Clear();
    };
}
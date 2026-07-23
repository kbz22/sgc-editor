#pragma once

#include <sgc/graphics/tileset.hpp>
#include <sgc/graphics/tiledimage.hpp>

#undef CreateWindow // avoid macro name conflict with sdl::CreateWindow

#include <sgc/sdl/sdl.hpp>
#include <windows.h>
#include <filesystem>

#include "defaults.hpp"

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
            
            bool CreateEmbeddedWindow();            

        protected:
            int m_tileWidth;
            int m_tileHeight;
            std::shared_ptr<graphics::Tileset> m_tileset = nullptr;        
            std::shared_ptr<graphics::IDrawable> m_drawableImage = nullptr;
            graphics::RenderContext m_renderContext{};
            
            HWND m_hostWindow = HWND(); //! remove the rest of win32 stuff once all is moved to section
            HWND m_sectionWindow = HWND();
            
            void DrawAll();            

            //std::shared_ptr<graphics::Tileset> LoadTileset(const std::filesystem::path& path);

        public:
            SgcView(HWND hwnd);
            ~SgcView();
            
            virtual void SetScreenSize(int width, int height);
            virtual void Render();

            virtual void SetTileset(std::shared_ptr<graphics::Tileset> tileset);
            virtual void SetRenderer(const graphics::RenderContext& context);

            graphics::RenderContext& GetRenderContext();

            std::shared_ptr<graphics::Tileset> GetTileset() const;
            sgc::graphics::PixelSize2D GetTileSize() const;
            SDL_Window* GetSdlWindow() const;
            
            sgc::math::uvec2 PixelsToTiles(sgc::math::uvec2 value) const;
            sgc::math::vec2 PixelsToTiles(sgc::math::vec2 value) const;
            sgc::math::fvec2 PixelsToTiles(sgc::math::fvec2 value) const;

            void Clear();            
            
            /* friend class sections::MapSection;
            friend class sections::TilesetSection; */
    };
}
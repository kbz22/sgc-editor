#pragma once

#include <sgc/sdl/sdl.hpp>
#include <sgc/graphics/tileset.hpp>
// #include <sgc/graphics/tiled_static_layer.hpp>
#include <sgc/graphics/tiledimage.hpp>

#include "defaults.hpp"

#include <windows.h>

namespace sgc_view
{
    using namespace sgc;

    class SgcView
    {
        private:
            bool m_sdlInitialized = false;            

            SDL_Window* m_sdlWindow = nullptr;            

            static LRESULT CALLBACK StaticPaneProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR id, DWORD_PTR data);            
            bool CreateEmbeddedRenderer();
            void InstallInputSubclass();

        protected:
            int m_tileWidth = defaults::tileSize;
            int m_tileHeight = defaults::tileSize;
            std::shared_ptr<graphics::Tileset> m_tileset = nullptr;
            // std::unique_ptr<graphics::TiledStaticLayer> m_layer = nullptr;            
            std::shared_ptr<graphics::TiledImage> m_tiledImage = nullptr;
            graphics::RenderContext m_renderContext = {};
            
            HWND m_hostWindow = HWND();
            HWND m_sectionWindow = HWND();

            virtual LRESULT HandleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
            void DrawAll();

        public:
            SgcView(HWND hwnd);
            ~SgcView();

            virtual void Render();
            void Clear();
            virtual bool LoadTileset(const std::wstring& path, int tileWidth = defaults::tileSize, int tileHeight = defaults::tileSize);
            virtual void SetScreenSize(int width, int height);
        
    };
}
#pragma once

#include <sgc/sdl/sdl.hpp>
#include <sgc/image/tileset.hpp>
#include <sgc/image/tiled_static_layer.hpp>

#include "defaults.hpp"

#include <windows.h>

namespace sgc
{
    class SgcView
    {
        private:
            HWND m_hostWindow = HWND();
            HWND m_sectionWindow = HWND();
            
            bool m_sdlInitialized = false;            

            SDL_Window* m_sdlWindow = nullptr;            

            static LRESULT CALLBACK StaticPaneProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR id, DWORD_PTR data);
            bool CreateEmbeddedRenderer();
            void InstallInputSubclass();

        protected:
            int m_tileWidth = defaults::tileSize;
            int m_tileHeight = defaults::tileSize;
            std::shared_ptr<image::Tileset> m_tileset = nullptr;
            std::unique_ptr<image::TiledStaticLayer> m_layer = nullptr;
            SDL_Renderer* m_renderer = nullptr;

        public:
            SgcView(HWND hwnd);
            ~SgcView();

            virtual void Render();
            void Clear();
            virtual bool LoadTileset(const std::wstring& path, int tileWidth = defaults::tileSize, int tileHeight = defaults::tileSize);
        
    };
}
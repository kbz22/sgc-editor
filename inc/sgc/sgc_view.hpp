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
        public:
            SgcView(HWND hwnd);
            ~SgcView();

            void Render();
            void Clear();
            bool LoadTileset(const std::wstring& path, int tileWidth = defaults::tileSize, int tileHeight = defaults::tileSize);

        private:

            HWND m_hostWindow = HWND();
            HWND m_sectionWindow = HWND();

            int m_tileWidth = defaults::tileSize;
            int m_tileHeight = defaults::tileSize;
            bool m_sdlInitialized = false;

            std::shared_ptr<image::Tileset> m_tileset;
            std::unique_ptr<image::TiledStaticLayer> m_layer;

            SDL_Window* m_sdlWindow = nullptr;
            SDL_Renderer* m_renderer = nullptr;

            static LRESULT CALLBACK StaticPaneProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR id, DWORD_PTR data);
            bool CreateEmbeddedRenderer();
            void InstallInputSubclass();

    };
}
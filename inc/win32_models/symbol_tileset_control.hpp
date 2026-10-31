#pragma once

#include "sgc_view/symbol_tileset_view.hpp"
#include "win32_program/pointer_lock.hpp"
#include "program/pointer_manager.hpp"
#include <functional>

namespace win32_models
{
    class SymbolTilesetControl
    {
        private:
            HWND m_hwnd;
            sgc::tile::TilePosition2D m_pointerOrigin = {0,0};
            std::unique_ptr<sgc_view::SymbolTilesetView> m_tilesetView;
            program::PointerManager &m_pointerManager;
            std::function<void(sgc::tile::TileSize2D)> m_sizeChangedCallback;

            win32_program::PointerLock m_isSelecting;
            win32_program::PointerLock m_isPanning;

            static bool m_classRegistered;

            static LRESULT CALLBACK SymbolTilesetViewStaticProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR id, DWORD_PTR data);
            LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

        public:
            SymbolTilesetControl(program::ProgramContext &programContext);

            void Init(HWND hwnd, program::ProgramContext &programContext);
            void Update();
            void Refresh(program::ProgramContext &programContext);
            std::vector<sgc::tile::TilePosition2D> ResetCurrentView();

            void SetSymbolSize(sgc::tile::TileSize2D size);
            void RegisterOnSizeChangedCallback(std::function<void(sgc::tile::TileSize2D)> sizeChangedCallback);

            sgc::tile::TileSize2D GetSymbolSize() const;
            sgc::tile::TileSize2D GetMaxSymbolSize() const;

            bool Empty() const;
    };
}
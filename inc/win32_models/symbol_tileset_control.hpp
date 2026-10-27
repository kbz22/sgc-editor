#pragma once

#include "sgc_view/symbol_tileset_view.hpp"

namespace win32_models
{
    class SymbolTilesetControl
    {
        private:
            HWND m_hwnd;
            std::unique_ptr<sgc_view::SymbolTilesetView> m_tilesetView;
            static bool m_classRegistered;

            static LRESULT CALLBACK SymbolTilesetViewStaticProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR id, DWORD_PTR data);
            LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

        public:
            SymbolTilesetControl(program::ProgramContext &programContext);

            void Init(HWND hwnd, program::ProgramContext &programContext);
            void Update();
            void Refresh(program::ProgramContext &programContext);

    };
}
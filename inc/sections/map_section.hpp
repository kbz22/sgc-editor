#pragma once

#include "sections/section.hpp"
#include "sgc_view/map_view.hpp"
#include <windows.h>

namespace program {
    struct ProgramContext;
}

namespace sections {

    class MapSection : public Section
    {
        private:
            std::unique_ptr<sgc_view::MapView> m_mapView = nullptr;
            bool m_isPainting = false;
            bool m_isPanning = false;
            bool m_checkTileBeforePainting = true;
            sgc::math::vec2 m_lastMousePos = { 0, 0 };
            sgc::math::vec2 m_selectionTileStart = { 0, 0 };

        protected:
            LRESULT HandleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;

        public:
            MapSection(program::ProgramContext& programContext);
            
            void Update() override;
            void HandleSectionResize() override;

            void Refresh(program::LayerManager& layerManager);

            void LoadTileset(const std::filesystem::path& path, int tileWidth, int tileHeight);
            void LoadMap(const std::filesystem::path& path);
            void SaveMap(const std::filesystem::path& path);
            
            void SetCheckTileBeforePainting(bool check);
    };

}
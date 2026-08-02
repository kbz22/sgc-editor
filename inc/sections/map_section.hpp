#pragma once

#include "sections/section.hpp"
#include "sgc_view/map_view.hpp"
#include "command/paint_command.hpp"
#include "editor_tools/brush.hpp"
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
            bool m_isCaptured = false;        
            sgc::math::vec2 m_lastMousePosPan = { 0, 0 };

            editor_tools::Brush m_brush;

        protected:
            LRESULT HandleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;

        public:
            MapSection(program::ProgramContext& programContext);
            
            void Update() override;
            void HandleSectionResize() override;

            void Refresh(program::ProgramContext& programContext) override;
            
            void SetCheckTileBeforePainting(bool check);
            void SetPaintMode(editor_tools::PaintMode paintMode);
    };

}
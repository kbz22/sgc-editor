#pragma once

#include "sections/section.hpp"
#include "sgc_view/map_view.hpp"
#include "command/paint_stroke_command.hpp"
#include <windows.h>

namespace program {
    struct ProgramContext;
}

namespace sections {

    class MapSection : public Section
    {
        private:
            std::unique_ptr<sgc_view::MapView> m_mapView = nullptr;
            std::unique_ptr<command::PaintStrokeCommand> m_paintStrokeCommand = nullptr;
            bool m_isPainting = false;
            bool m_isPanning = false;
            bool m_isCaptured = false;
            bool m_checkTileBeforePainting = true;            
            sgc::math::vec2 m_lastMousePosPan = { 0, 0 };
            sgc::math::vec2 m_lastMousePosBrush = { 0, 0 };
            sgc::math::vec2 m_selectionTileStart = { 0, 0 };

        protected:
            LRESULT HandleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;

        public:
            MapSection(program::ProgramContext& programContext);
            
            void Update() override;
            void HandleSectionResize() override;

            void Refresh(program::ProgramContext& programContext) override;
            
            void SetCheckTileBeforePainting(bool check);
    };

}
#pragma once

#include "sections/section.hpp"
#include <sgc/math/vector.hpp>
#include <sgc/tile/tile.hpp>
#include <optional>
#include <unordered_map>

namespace sections {

    enum class StatusField {
        CursorPosition = 0,
        TileId = 1,
        SelectionSize = 2,
        Count
    };

    class StatusSection : public Section
    {
        private:            
            std::unordered_map<StatusField, std::wstring> m_statusTexts;
            const int m_partsWidths[static_cast<int>(StatusField::Count)] = {
                100,
                200,
                -1
            };
            std::wstring m_padding = L"  ";

        public:
            StatusSection(program::ProgramContext& programContext);
            ~StatusSection();

            void Update() override;
            void Refresh(program::ProgramContext& programContext) override;
            void HandleSectionResize() override;

            void SetStatusText(StatusField field, const std::wstring& text);
            
            void SetStatusCursorPosition(const sgc::graphics::PixelPosition2D& position);
            void SetStatusTileId(std::optional<sgc::tile::TileId> tileId);
            void SetStatusSelectionSize(const sgc::math::vec2& size);

    };

}
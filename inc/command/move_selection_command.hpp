#pragma once

#include "command/icommand.hpp"
#include "sgc_view/map_view.hpp"
#include <sgc/tile/tile.hpp>

namespace command
{
    class MoveSelectionCommand : public ICommand
    {
        private:
            sgc_view::MapView& m_mapView;
            sgc::tile::TilePosition2D m_mouseStartpoint;
            sgc::tile::TilePosition2D m_mouseEndpoint;
            sgc::tile::TileSize2D m_selectionSize;

        public:
            MoveSelectionCommand(
                sgc_view::MapView& mapView,
                sgc::tile::TilePosition2D mouseStartPosition,
                sgc::tile::TilePosition2D mouseEndPosition,
                sgc::tile::TileSize2D selectionSize
            );
            MoveSelectionCommand(const MoveSelectionCommand& other);

            void Execute() override;
            void Commit() override;
            void Undo() override;
    };
}
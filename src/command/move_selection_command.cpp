#include "command/move_selection_command.hpp"

command::MoveSelectionCommand::MoveSelectionCommand(sgc_view::MapView& mapView, sgc::tile::TilePosition2D selectionStartPosition, sgc::tile::TilePosition2D selectionEndPosition, sgc::tile::TileSize2D selectionSize) :
    m_mapView{mapView},
    m_selectionStartpoint{selectionStartPosition},
    m_selectionEndpoint{selectionEndPosition},
    m_selectionSize{selectionSize}
{}

command::MoveSelectionCommand::MoveSelectionCommand(const MoveSelectionCommand& other) = default;

void command::MoveSelectionCommand::Execute()
{
    auto tileSize = m_mapView.GetTileSize();

    m_mapView.SetSelectionPositionInPixels({
        m_selectionEndpoint.x * tileSize.x,
        m_selectionEndpoint.y * tileSize.y
    });
    m_mapView.SetSelectionSizeInPixels({
        m_selectionSize.x * tileSize.x,
        m_selectionSize.y * tileSize.y
    });

    return;
}

void command::MoveSelectionCommand::Commit()
{
    return;
}

void command::MoveSelectionCommand::Undo()
{
    auto tileSize = m_mapView.GetTileSize();

    m_mapView.SetSelectionPositionInPixels({
        m_selectionStartpoint.x * tileSize.x,
        m_selectionStartpoint.y * tileSize.y
    });
    m_mapView.SetSelectionSizeInPixels({
        m_selectionSize.x * tileSize.x,
        m_selectionSize.y * tileSize.y
    });

    return;
}
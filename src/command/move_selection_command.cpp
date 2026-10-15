#include "command/move_selection_command.hpp"

command::MoveSelectionCommand::MoveSelectionCommand(sgc_view::MapView& mapView, sgc::tile::TilePosition2D mouseStartPosition, sgc::tile::TilePosition2D mouseEndPosition, sgc::tile::TileSize2D selectionSize) :
    m_mapView{mapView},
    m_mouseStartpoint{mouseStartPosition},
    m_mouseEndpoint{mouseEndPosition},
    m_selectionSize{selectionSize}
{}

command::MoveSelectionCommand::MoveSelectionCommand(const MoveSelectionCommand& other) = default;

void command::MoveSelectionCommand::Execute()
{
    auto tileSize = m_mapView.GetTileSize();

    m_mapView.SetSelectionPositionInPixels({
        m_mouseEndpoint.x * tileSize.x,
        m_mouseEndpoint.y * tileSize.y
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
        m_mouseStartpoint.x * tileSize.x,
        m_mouseStartpoint.y * tileSize.y
    });
    m_mapView.SetSelectionSizeInPixels({
        m_selectionSize.x * tileSize.x,
        m_selectionSize.y * tileSize.y
    });

    return;
}
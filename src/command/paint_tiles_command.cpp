#include "command/paint_tiles_command.hpp"

command::PaintTilesCommand::PaintTilesCommand(
    std::vector<TileChange> tileChanges,
    sgc::data::ITileStorage &tileStorage
) :
    m_tileChanges{std::move(tileChanges)},
    m_tileStorage{tileStorage}
{}

void command::PaintTilesCommand::Execute()
{
    for (const auto& change : m_tileChanges) {
        m_tileStorage.SetTileAt(change.position, change.newTileId);
    }
}

void command::PaintTilesCommand::Undo()
{
    for (const auto& change : m_tileChanges) {
        m_tileStorage.SetTileAt(change.position, change.previousTileId);
    }
}
#include "command/paint_stroke_command.hpp"

command::PaintStrokeCommand::PaintStrokeCommand(sgc::data::ITileStorage &tileStorage) :
    m_tileStorage{tileStorage}
{}

void command::PaintStrokeCommand::ExecuteTileChange(const TileChange &tileChange)
{
    if(m_tileChanges.find(tileChange.position) == m_tileChanges.end()) {
        m_tileChanges[tileChange.position] = tileChange;
    }
    else {
        auto &existingTileChange = m_tileChanges[tileChange.position];
        existingTileChange.newTileId = tileChange.newTileId;
    }

    m_tileStorage.SetTileAt(tileChange.position, tileChange.newTileId);
}

void command::PaintStrokeCommand::Execute()
{
    for (const auto& [position, change] : m_tileChanges) {
        m_tileStorage.SetTileAt(position, change.newTileId);
    }
}

void command::PaintStrokeCommand::Undo()
{
    for (const auto& [position, change] : m_tileChanges) {
        m_tileStorage.SetTileAt(position, change.previousTileId);
    }
}
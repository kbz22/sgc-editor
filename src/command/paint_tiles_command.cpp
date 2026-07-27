#include "command/paint_tiles_command.hpp"
#include "program/program.hpp"

command::PaintTilesCommand::PaintTilesCommand(
    std::vector<TileChange> tileChanges,
    sgc::data::ITileStorage &tileStorage
) :
    m_tileChanges{std::move(tileChanges)},
    m_tileStorage{tileStorage}
{}

void command::PaintTilesCommand::Execute()
{    
    auto mapDocument = program::GetProgramContext().fileManager->GetSelectedDocument();
    if(mapDocument != nullptr) 
    {
        for (const auto& change : m_tileChanges) {
            m_tileStorage.SetTileAt(change.position, change.newTileId);
        }

        mapDocument->SetDirty(true);
    }
}

void command::PaintTilesCommand::Undo()
{
    for (const auto& change : m_tileChanges) {
        m_tileStorage.SetTileAt(change.position, change.previousTileId);
    }
}
#pragma once

#include "command/icommand.hpp"
#include "command/tile_change.hpp"
#include <sgc/data/itilestorage.hpp>
#include <vector>
#include <optional>

namespace command {

    class PaintTilesCommand : public ICommand {

        private:
            std::vector<TileChange> m_tileChanges;
            sgc::data::ITileStorage &m_tileStorage;

        public:
            PaintTilesCommand(
                std::vector<TileChange> tileChanges,
                sgc::data::ITileStorage &tileStorage
            );
            virtual ~PaintTilesCommand() = default;

            void Execute() override;
            void Undo() override;

    };

}
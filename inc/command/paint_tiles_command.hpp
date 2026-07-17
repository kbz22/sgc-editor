#pragma once

#include "command/icommand.hpp"
#include <sgc/math/value.hpp>
#include <sgc/graphics/tileset.hpp>
#include <sgc/data/itilestorage.hpp>
#include <vector>
#include <memory>
#include <optional>

namespace command {

    struct TileChange {
        sgc::math::vec2 position;
        std::optional<sgc::tile::TileId> previousTileId;
        std::optional<sgc::tile::TileId> newTileId;
    };

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
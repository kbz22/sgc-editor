#pragma once

#include "command/icommand.hpp"
#include "file/map_document.hpp"
#include <sgc/tile/tile.hpp>

#include <unordered_map>
#include <optional>

namespace command
{
    struct MultilayerTileChange
    {
        size_t layerIndex;
        sgc::tile::TilePosition2D tilePosition;
        std::optional<sgc::tile::TileId> oldTileId;
        std::optional<sgc::tile::TileId> newTileId;
    };

    using MultilayerTileChangesType = std::unordered_map<
        size_t,
        std::unordered_map<sgc::tile::TilePosition2D, MultilayerTileChange>
    >;

    class PaintSelectionCommand : public ICommand
    {
        private:
            file::MapDocument& m_mapDocument;
            MultilayerTileChangesType m_tileChanges;

        public:
            PaintSelectionCommand(
                file::MapDocument& mapDocument,
                const MultilayerTileChangesType& tileChanges
            );

            void Execute() override;
            void Commit() override;
            void Undo() override;
    };
}
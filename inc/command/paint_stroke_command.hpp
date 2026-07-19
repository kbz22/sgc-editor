#pragma once

#include "command/icommand.hpp"
#include "command/tile_change.hpp"
#include <sgc/math/value.hpp>
#include <sgc/data/itilestorage.hpp>
#include <unordered_map>

namespace command {

    class PaintStrokeCommand : public ICommand {

        private:
            std::unordered_map<sgc::math::vec2, TileChange> m_tileChanges;
            // sgc::data::ITileStorage &m_tileStorage;
            size_t m_activeLayerIndex;

        public:
            PaintStrokeCommand(
                // sgc::data::ITileStorage &tileStorage
                size_t activeLayerIndex
            );
            virtual ~PaintStrokeCommand() = default;

            void ExecuteTileChange(const TileChange &tileChange);

            void Execute() override;
            void Undo() override;

    };

}
#pragma once

#include "command/icommand.hpp"
#include "command/tile_change.hpp"
#include <sgc/math/value.hpp>
#include <sgc/data/itilestorage.hpp>
#include <unordered_map>

namespace command {

    class PaintCommand : public ICommand {

        private:
            std::unordered_map<sgc::math::vec2, TileChange> m_tileChanges;
            size_t m_activeLayerIndex;

        public:
            PaintCommand(
                size_t activeLayerIndex
            );
            virtual ~PaintCommand() = default;

            void ExecuteTileChange(const TileChange &tileChange);

            void Execute() override;
            void Undo() override;

    };

}
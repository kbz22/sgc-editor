#pragma once

#include "command/icommand.hpp"
#include "command/tile_change.hpp"
#include "file/map_document.hpp"
#include <sgc/math/value.hpp>
#include <sgc/data/itilestorage.hpp>
#include <unordered_map>

namespace command {

    class PaintCommand : public ICommand {

        private:
            std::unordered_map<sgc::math::vec2, TileChange> m_tileChanges;
            file::MapDocument* m_mapDocument;
            size_t m_activeLayerIndex;

        public:
            PaintCommand(
                file::MapDocument* mapDocument,
                size_t activeLayerIndex
            );
            PaintCommand(const PaintCommand& other);
            
            virtual ~PaintCommand() = default;

            void ExecuteTileChange(const TileChange &tileChange);
            void ExecuteTileChange(const std::unordered_map<sgc::math::vec2, TileChange> &tileChanges);
            void UndoTileChanges();
            file::MapDocument* GetMapDocument() const;

            void Execute() override;
            void Commit() override;
            void Undo() override;

    };

}
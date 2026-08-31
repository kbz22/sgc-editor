#pragma once

#include "command/icommand.hpp"
#include <memory>
// #include <sgc/data/itilestorage.hpp>
#include "program/layer_manager.hpp"

namespace command {

    class LayerRemoveCommand : public ICommand {

        private:
            size_t m_removedLayerIndex = 0;
            program::LayerItem m_removedLayerItem;

        public:
            LayerRemoveCommand(size_t removedLayerIndex);
            LayerRemoveCommand(const LayerRemoveCommand& other);
            virtual ~LayerRemoveCommand() = default;

            void Execute() override;
            void Commit() override;
            void Undo() override;

    };

}
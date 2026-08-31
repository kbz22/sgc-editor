#pragma once

#include "command/icommand.hpp"
#include <cstdint>

namespace command {

    class LayerAddCommand : public ICommand {

        private:
            size_t m_addedLayerIndex = 0;

        public:
            LayerAddCommand() = default;
            LayerAddCommand(const LayerAddCommand& other);
            virtual ~LayerAddCommand() = default;

            void Execute() override;
            void Commit() override;
            void Undo() override;

    };

}
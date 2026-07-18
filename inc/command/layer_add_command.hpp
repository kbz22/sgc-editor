#pragma once

#include "command/icommand.hpp"
#include <cstdint>

namespace command {

    class LayerAddCommand : public ICommand {

        private:
            size_t m_addedLayerIndex = 0;

        public:
            LayerAddCommand() = default;
            virtual ~LayerAddCommand() = default;

            void Execute() override;
            void Undo() override;

    };

}
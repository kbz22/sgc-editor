#pragma once

#include "command/icommand.hpp"
#include <memory>
#include <sgc/data/itilestorage.hpp>

namespace command {

    class LayerRemoveCommand : public ICommand {

        private:
            size_t m_removedLayerIndex = 0;
            std::shared_ptr<sgc::data::ITileStorage> m_removedLayerStorage = nullptr;

        public:
            LayerRemoveCommand() = default;
            virtual ~LayerRemoveCommand() = default;

            void Execute() override;
            void Undo() override;

    };

}
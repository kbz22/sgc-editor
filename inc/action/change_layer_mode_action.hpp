#pragma once

#include "action/action.hpp"
#include "program/editor_mode.hpp"

namespace action {

    class ChangeLayerModeAction : public Action
    {
        private:
            program::EditorLayerMode m_layerMode;

        public:
            ChangeLayerModeAction(program::EditorLayerMode layerMode);

            void Execute(program::ProgramContext& context) override;
    };

}
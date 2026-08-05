#pragma once

#include "action/action.hpp"
#include "editor_tools/brush.hpp"

namespace action {

    class ChangeBrushEraseModeAction : public Action
    {
        private:
            editor_tools::EraserMode m_eraserMode;
            
        public:
            ChangeBrushEraseModeAction(editor_tools::EraserMode eraserMode);

            void Execute(program::ProgramContext& context) override;
    };

}
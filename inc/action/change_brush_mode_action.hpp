#pragma once

#include "action/action.hpp"
#include "editor_tools/brush.hpp"

namespace action {

    class ChangeBrushModeAction : public Action
    {
        private:
            editor_tools::PaintMode m_brushMode;

        public:
            ChangeBrushModeAction(editor_tools::PaintMode brushMode);

            void Execute(program::ProgramContext& context) override;
    };

}
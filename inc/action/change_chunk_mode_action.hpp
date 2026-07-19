#pragma once

#include "action/action.hpp"
#include "program/editor_mode.hpp"

namespace action {

    class ChangeChunkModeAction : public Action
    {
        private:
            program::EditorChunkMode m_chunkMode;

        public:
            ChangeChunkModeAction(program::EditorChunkMode chunkMode);

            void Execute(program::ProgramContext& context) override;
    };

}
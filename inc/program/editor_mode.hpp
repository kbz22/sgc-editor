#pragma once

namespace program {

    struct ProgramContext;

    enum class EditorLayerMode {
        SingleLayer = 0,
        MultiLayer = 1,
        SingleImage = 2
    };

    enum class EditorChunkMode {
        FixedChunks = 0,
        DynamicChunks = 1
    };

    void MultiLayerModeSetup(ProgramContext& context);
    void SingleImageModeSetup(ProgramContext& context);

}
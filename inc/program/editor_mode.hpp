#pragma once

#include <cstdint>

namespace program {

    class ProgramContext;

    enum class EditorLayerMode {
        SingleLayer = 0,
        MultiLayer = 1,
        SingleImage = 2
    };

    enum class EditorChunkMode {
        FixedChunks = 0,
        DynamicChunks = 1
    };

    enum class EditorGridMode : uint8_t {
        NoGrid = 0,
        TileGrid = 1,
        ChunkGrid = 2
    };

    inline bool HasFlag(EditorGridMode mode, EditorGridMode flag) {
        return (static_cast<uint8_t>(mode) & static_cast<uint8_t>(flag)) != 0;
    }

    void MultiLayerModeSetup(ProgramContext& context);
    void SingleImageModeSetup(ProgramContext& context);

}
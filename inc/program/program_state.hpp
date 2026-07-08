#pragma once

namespace program {

    enum class EditorState {
        Default = 0,
        HwndInitialized = 1,
        NewMap = 2,
        MapLoaded = 3,
        Resized = 4
    };

}
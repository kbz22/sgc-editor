#pragma once

namespace program {   

    enum EditorState
    {
        Inactive,
        NoMap,
        NoTileset,        
        Active
    };

    struct ProgramContext
    {
        EditorState editorState;

    };

    ProgramContext& GetProgramContext();

}
#pragma once

namespace editor_tools
{
    enum class PaintMode
    {
        Brush,
        Rectangle,
        Fill,
        Select,
        ChunkRemover,
        TilePicker,
        MoveSelection,
        Scatter,
    };

    enum class SelectionMode
    {
        SingleLayer,
        AllLayers,
        VisibleLayers
    };
}
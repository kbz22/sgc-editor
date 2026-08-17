#pragma once

#include <unordered_map>
#include <vector>
#include <sgc/tile/tile.hpp>
#include <win32_program/clipboard.hpp>

namespace win32_program {    

    struct MapSelection 
    {
        sgc::math::vec2 startPoint;
        sgc::math::vec2 size;
        std::unordered_map<size_t, std::vector<sgc::tile::TileId>> layerTiles;
    };

    template<>
    struct ClipboardSerializer<MapSelection>{
        static constexpr std::wstring_view FormatName = L"SGCME.MapSelection";
        static std::vector<uint8_t> Serialize(const MapSelection& object);
        static MapSelection Deserialize(std::span<const std::byte> data);
    };

}
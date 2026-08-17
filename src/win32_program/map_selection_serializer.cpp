#include "win32_program/clipboard.hpp"
#include "win32_program/map_selection_serializer.hpp"
#include <sgc/data/serialization.hpp>

std::vector<uint8_t> win32_program::ClipboardSerializer<win32_program::MapSelection>::Serialize(const MapSelection& object)
{
    sgc::data::BinaryWriter bw{};

    bw.Write(object.startPoint);
    bw.Write(object.size);
    bw.Write(object.layerTiles.size());

    for(auto &[layerIndex, tileVector] : object.layerTiles)
    {
        bw.Write(layerIndex);
        bw.Write(tileVector.size());

        for(auto tile : tileVector)
            bw.Write(tile);
    }

    return bw.buffer;
}

win32_program::MapSelection win32_program::ClipboardSerializer<win32_program::MapSelection>::Deserialize(std::span<const std::byte> data)
{
    sgc::data::BinaryReader br{};
    MapSelection selection{};
    br.data = reinterpret_cast<const uint8_t*>(data.data());

    selection.startPoint = br.Read<sgc::math::vec2>();
    selection.size = br.Read<sgc::math::vec2>();
    size_t layerEntryCount = br.Read<size_t>();

    for(int i=0; i<layerEntryCount; i++)
    {
        size_t layerIndex = br.Read<size_t>();
        size_t tileCount = br.Read<size_t>();
        selection.layerTiles[layerIndex] = std::vector<sgc::tile::TileId>{};
        selection.layerTiles[layerIndex].reserve(tileCount);

        for(int j=0;j<tileCount;j++)
            selection.layerTiles[layerIndex].push_back(br.Read<sgc::tile::TileId>());
    }

    return selection;
}
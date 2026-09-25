#include "settings/isetting.hpp"
#include <sgc/data/serialization.hpp>

std::vector<uint8_t> settings::SettingHeader::GetBytes() const
{
    sgc::data::BinaryWriter writer;

    writer.Write(static_cast<uint32_t>(type));
    writer.Write(key);
    writer.Write(size);

    return writer.buffer;
}

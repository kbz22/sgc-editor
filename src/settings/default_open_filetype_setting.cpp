#include "settings/default_open_filetype_setting.hpp"
#include <sgc/data/serialization.hpp>

settings::DefaultOpenFiletypeSetting::DefaultOpenFiletypeSetting(file::FileType defaultFileType) :
    m_value(defaultFileType),
    m_newValue(defaultFileType)
{
}

std::vector<uint8_t> settings::DefaultOpenFiletypeSetting::GetBytes() const
{
    sgc::data::BinaryWriter writer;

    writer.Write(m_value);

    return writer.buffer;
}

void settings::DefaultOpenFiletypeSetting::LoadFromBytes(const std::vector<uint8_t>& bytes)
{
    sgc::data::BinaryReader reader(bytes.data());

    m_value = reader.Read<file::FileType>();
    m_newValue = m_value;
}

file::FileType settings::DefaultOpenFiletypeSetting::GetValue() const
{
    return m_value;
}

void settings::DefaultOpenFiletypeSetting::SetValue(file::FileType newValue)
{
    m_newValue = newValue;
}

void settings::DefaultOpenFiletypeSetting::Commit()
{
    m_value = m_newValue;
}
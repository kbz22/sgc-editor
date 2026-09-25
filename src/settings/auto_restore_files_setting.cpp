#include "settings/auto_restore_files_setting.hpp"
#include <sgc/data/serialization.hpp>

settings::AutoRestoreFilesSetting::AutoRestoreFilesSetting(bool defaultValue) :
    m_value(defaultValue),
    m_newValue(defaultValue)
{
}

std::vector<uint8_t> settings::AutoRestoreFilesSetting::GetBytes() const
{
    sgc::data::BinaryWriter writer;

    writer.Write(m_value);

    return writer.buffer;
}

void settings::AutoRestoreFilesSetting::LoadFromBytes(const std::vector<uint8_t>& bytes)
{
    sgc::data::BinaryReader reader(bytes.data());

    m_value = reader.Read<bool>();
    m_newValue = m_value;
}

bool settings::AutoRestoreFilesSetting::GetValue() const
{
    return m_value;
}

void settings::AutoRestoreFilesSetting::SetValue(bool newValue)
{
    m_newValue = newValue;
}

void settings::AutoRestoreFilesSetting::Commit()
{
    m_value = m_newValue;
}
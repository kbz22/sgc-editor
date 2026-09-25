#include "settings/default_open_filetype_setting.hpp"

settings::DefaultOpenFiletypeSetting::DefaultOpenFiletypeSetting(file::FileType defaultFileType) :
    m_value(defaultFileType),
    m_newValue(defaultFileType)
{
}

std::vector<uint8_t> settings::DefaultOpenFiletypeSetting::GetBytes() const
{
    //! stub
    return std::vector<uint8_t>{};
}

void settings::DefaultOpenFiletypeSetting::LoadFromBytes(const std::vector<uint8_t>& bytes)
{
    //! also stub
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
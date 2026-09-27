#include "settings/default_open_filetype_setting.hpp"
#include <sgc/data/serialization.hpp>

settings::DefaultOpenFiletypeSetting::DefaultOpenFiletypeSetting(file::FileType defaultFileType) :
    m_value(defaultFileType),
    m_newValue(defaultFileType)
{
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

settings::Key settings::DefaultOpenFiletypeSetting::GetKey()
{
    return Key::DefaultOpenFiletypeSetting;
}
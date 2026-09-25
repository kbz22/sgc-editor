#include "settings/auto_restore_files_setting.hpp"

settings::AutoRestoreFilesSetting::AutoRestoreFilesSetting(bool defaultValue) :
    m_value(defaultValue),
    m_newValue(defaultValue)
{
}

std::vector<uint8_t> settings::AutoRestoreFilesSetting::GetBytes() const
{
    return std::vector<uint8_t>{}; //! stub
}

void settings::AutoRestoreFilesSetting::LoadFromBytes(const std::vector<uint8_t>& bytes)
{
    //! stub
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
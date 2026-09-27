#include "settings/auto_restore_files_setting.hpp"
#include <sgc/data/serialization.hpp>

settings::AutoRestoreFilesSetting::AutoRestoreFilesSetting(bool defaultValue) :
    m_value(defaultValue),
    m_newValue(defaultValue)
{
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

settings::Key settings::AutoRestoreFilesSetting::GetKey()
{
    return Key::AutoRestoreFilesSetting;
}
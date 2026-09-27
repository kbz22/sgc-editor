#pragma once

#include "settings/isetting.hpp"

namespace settings 
{
    class AutoRestoreFilesSetting : public ISetting
    {
        private:
            bool m_value;
            bool m_newValue;

        public:
            AutoRestoreFilesSetting(bool initialValue);
            
            bool GetValue() const;
            void SetValue(bool newValue);

            void Commit() override;
            Key GetKey() override;
    };
}
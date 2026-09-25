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

            std::vector<uint8_t> GetBytes() const override;
            void LoadFromBytes(const std::vector<uint8_t>& bytes) override;

            bool GetValue() const;
            void SetValue(bool newValue);

            void Commit() override;
    };
}
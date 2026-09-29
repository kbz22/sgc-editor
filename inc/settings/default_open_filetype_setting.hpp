#pragma once

#include "settings/isetting.hpp"
#include "file/ifile.hpp"

namespace settings 
{
    class DefaultOpenFiletypeSetting : public ISetting
    {
        private:
            file::FileType m_value;
            file::FileType m_newValue;

        public:
            DefaultOpenFiletypeSetting(file::FileType defaultFileType = file::FileType::Map);

            file::FileType GetValue() const;
            void SetValue(file::FileType newValue);

            void Commit() override;
            Key GetKey() override;
    };
}
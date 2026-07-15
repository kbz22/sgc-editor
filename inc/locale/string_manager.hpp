#pragma once

#include "locale/stringid.hpp"

#include <unordered_map>
#include <string>

namespace locale {

    struct EnumClassHash
    {
        template<typename T>
        size_t operator()(T t) const
        {
            return static_cast<size_t>(t);
        }
    };

    class StringManager
    {
        private:
            std::unordered_map<StringId, std::wstring, EnumClassHash> m_strings;

        public:
            StringManager();
            const std::wstring& Get(StringId id) const;
    
    };

}
#pragma once

#include "settings/settings_types.hpp"
#include <unordered_map>
#include <string>

namespace settings 
{
    class SettingsJsonStringLookup
    {
        private:
            std::unordered_map<Key, std::wstring> m_strings;
            std::wstring m_valueMissing = L"value-missing";

        public:
            SettingsJsonStringLookup();

            std::wstring Get(Key key);
    };
}
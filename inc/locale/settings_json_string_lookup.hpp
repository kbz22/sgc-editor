#pragma once

#include "settings/settings_types.hpp"
#include <unordered_map>
#include <string>

namespace locale 
{
    class SettingsJsonStringLookup
    {
        private:
            std::unordered_map<settings::Key, std::string> m_settingsStrings;
            std::string m_valueMissing = "value-missing";

        public:
            SettingsJsonStringLookup();

            std::string Get(settings::Key key);
    };
}
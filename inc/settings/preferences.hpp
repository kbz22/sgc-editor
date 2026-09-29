#pragma once

#include <nlohmann/json.hpp>
#include "settings/settings_json_string_lookup.hpp"

namespace settings
{
    class Preferences
    {
        private:
            nlohmann::json m_json{};
            SettingsJsonStringLookup m_stringLookup{};

        public:
            void Load();
            void Save();

            template<typename T>
            void Set(T const& setting);
            
            template<typename T>
            T Get();
    };
}
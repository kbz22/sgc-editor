#pragma once

#include <nlohmann/json.hpp>
#include "locale/settings_json_string_lookup.hpp"
#include "settings/isetting.hpp"

namespace settings
{
    class Preferences
    {
        private:
            nlohmann::json m_json{};
            locale::SettingsJsonStringLookup m_stringLookup{};
            std::wstring m_preferencesFileName = L"preferences.json";

        public:
            void Load();
            void Save();

            template<typename T>
            void Set(T const &setting);

            template<typename T>
            void Get(T &setting);
            void Get(settings::ISetting* setting);
    };
}
#pragma once

#include <nlohmann/json.hpp>

namespace settings
{
    class Preferences
    {
        private:
            nlohmann::json m_json;

        public:
            void Load();
            void Save();

            template<typename T>
            void Set(std::wstring setting, T const& value);
            template<typename T>
            T Get(std::wstring setting);
    };
}
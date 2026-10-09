#pragma once

#include "sections/map_section.hpp"
#include "settings/settings_types.hpp"
#include <unordered_map>

namespace locale
{
    class JsonStringLookup
    {
        private:
            std::unordered_map<settings::Key, std::string> m_settingsStrings;
            std::unordered_map<sections::PointerType, std::string> m_pointerTypeStrings;
            std::unordered_map<sections::PointerBehaviour, std::string> m_behaviourTypeStrings;

        public:
            JsonStringLookup();

            template<typename T>
            std::string Get(T value);

            template<typename T>
            T Resolve(std::string string);
    };
}
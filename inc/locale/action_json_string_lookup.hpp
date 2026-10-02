#pragma once

#include "action/action.hpp"
#include "locale/string_lookup.hpp"
#include <unordered_map>
#include <string>
#include <vector>

namespace locale
{
    class ActionJsonStringLookup
    {
        private:
            static std::unordered_map<action::ActionType, std::string> m_strings;

        public:
            static std::string Get(action::ActionType actionId);
            static action::ActionType Get(std::string key);
    };
}
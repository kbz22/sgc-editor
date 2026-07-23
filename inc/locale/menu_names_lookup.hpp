#pragma once

#include "locale/stringid.hpp"
#include "action/action_description.hpp"
#include <unordered_map>

namespace locale {

    class MenuNamesLookup
    {
        private:
            std::unordered_map<action::MenuId, locale::StringId> m_strings;

        public:
            MenuNamesLookup();
            const locale::StringId& Get(action::MenuId id) const;
    
    };

}
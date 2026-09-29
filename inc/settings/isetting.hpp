#pragma once

#include "settings/settings_types.hpp"
#include <cstdint>
#include <vector>

namespace settings 
{
    struct ISetting
    {
        virtual ~ISetting() = default;

        // Update
        virtual void Commit() = 0;
        virtual Key GetKey() const = 0;
    };

}
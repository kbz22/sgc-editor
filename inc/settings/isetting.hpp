#pragma once

#include "settings/settings_category.hpp"
#include <cstdint>
#include <vector>

namespace settings {

    struct SettingHeader
    {
        SettingCategory type;
        uint32_t key;
        uint32_t size;

        std::vector<uint8_t> GetBytes() const;
    };

    struct ISetting
    {
        virtual ~ISetting() = default;

        // Serialization
        virtual std::vector<uint8_t> GetBytes() const = 0;
        virtual void LoadFromBytes(const std::vector<uint8_t>& bytes) = 0;

        // Update
        virtual void Commit() = 0;
    };

}
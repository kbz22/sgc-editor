#pragma once

#include <cstdint>
#include <vector>

namespace program {

    struct ISetting
    {
        virtual ~ISetting() = default;

        virtual std::vector<uint8_t> GetBytes() const = 0;
        virtual void LoadFromBytes(const std::vector<uint8_t>& bytes) = 0;
    };

}
#pragma once

#include <vector>
#include <memory>

namespace file {    

    template<typename T>
    struct DocumentSerializer
    {
        static std::vector<uint8_t> Serialize(T* document);
        static std::unique_ptr<T> Deserialize(const std::vector<uint8_t>& bytes);
    };

}
#pragma once

#include <stdexcept>

namespace program
{
    class TileSizeException : public std::runtime_error
    {
        public:
            TileSizeException(const std::string& message) : std::runtime_error(message) {}
    };

    class AssetLoadException : public std::runtime_error
    {
        public:
            AssetLoadException(const std::string& message) : std::runtime_error(message) {}
    };
    
}
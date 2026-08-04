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

    class AssetCacheException : public std::runtime_error
    {
        public:
            AssetCacheException(const std::string& message) : std::runtime_error(message) {}
    };

    class StringNotFoundException : public std::runtime_error
    {
        public:
            StringNotFoundException(const std::string& message) : std::runtime_error(message) {}
    };

    class FileLoadException : public std::runtime_error
    {
        public:
            FileLoadException(const std::string& message) : std::runtime_error(message) {}
    };

    class ResourceLoadException : public std::runtime_error
    {
        public:
            ResourceLoadException(const std::string& message) : std::runtime_error(message) {}
    };
    
}
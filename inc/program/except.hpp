#pragma once

#include <stdexcept>
#include <filesystem>
#include <vector>

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

    class SessionRestoreException : public std::runtime_error
    {
        private:
            std::vector<std::filesystem::path> m_failedFiles;

        public:
            explicit SessionRestoreException(const std::string& message, std::vector<std::filesystem::path> failedFiles) :
                std::runtime_error(message),
                m_failedFiles(std::move(failedFiles)) {}

            const std::vector<std::filesystem::path>& GetFailedFiles() const
            {
                return m_failedFiles;
            }

    };
    
}
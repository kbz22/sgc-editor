#pragma once

#include <string>

namespace file {

    class IFile
    {
        public:
            virtual void New() = 0;
            virtual void Open() = 0;
            virtual void Save() = 0;
            virtual void Close() = 0;

            virtual std::wstring GetExtension() const = 0;
            virtual std::wstring GetFileName() const = 0;
    };

}
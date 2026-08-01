#pragma once

#include <string>

namespace file {

    class ITreeViewListable
    {
        public:
            virtual ~ITreeViewListable() = default;

            virtual const std::wstring& GetName() const = 0;
            virtual bool IsContainer() const = 0;
            virtual bool IsActivable() const = 0;
            virtual bool IsDirty() const = 0;
    };

}
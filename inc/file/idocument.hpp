#pragma once

#include <string>
#include "file/itreeviewlistable.hpp"

namespace file {

    class IDocument : public ITreeViewListable
    {
        public:
            virtual ~IDocument() = default;                        
            virtual bool IsActivable() const = 0;
            virtual bool IsDirty() const = 0;
            virtual bool IsContainer() const = 0;
            virtual const std::wstring& GetName() const = 0;
            virtual ListableType GetListableType() const = 0;
    };

}
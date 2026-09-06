#pragma once

#include <string>

namespace file {

    enum class ListableType {
        MapDocument = 0,
        TilesetDocument = 1,
        PackageFile = 2
    };

    class ITreeViewListable
    {
        public:
            virtual ~ITreeViewListable() = default;

            virtual const std::wstring& GetName() const = 0;
            virtual bool IsContainer() const = 0;            
            virtual bool IsActivable() const = 0;
            virtual bool IsDirty() const = 0;
            virtual ListableType GetListableType() const = 0;
    };

}
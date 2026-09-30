#pragma once

#include "file/idocument.hpp"
#include "file/package_file.hpp"
#include "file/itreeviewlistable.hpp"
#include <string>

namespace file {

    class PackageDocument : public IDocument
    {
        private:
            PackageFile& m_packageFile;            
            
        public:
            PackageDocument(PackageFile& packageFile);

            bool IsActivable() const override;
            bool IsDirty() const override;
            bool IsContainer() const override;
            const std::wstring& GetName() const override;

            ListableType GetListableType() const override;
    };

}
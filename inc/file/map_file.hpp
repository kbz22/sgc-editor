#pragma once

#include "file/ifile.hpp"
#include "defaults.hpp"

namespace file {

    class MapFile : public IFile
    {        
        private:
            std::wstring m_fileName{};
            std::wstring m_extension{defaults::MapFileExtension};

        public:
            MapFile() = default;
            virtual ~MapFile() = default;

            void New() override;
            void Open() override;
            void Save() override;
            void Close() override;

            std::wstring GetExtension() const override;
            std::wstring GetFileName() const override;
    };

}
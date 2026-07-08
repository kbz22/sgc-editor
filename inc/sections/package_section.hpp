#pragma once

#include "sections/section.hpp"

namespace sections {

    class PackageSection : public Section
    {
        public:
            PackageSection(win32_program::Win32Context& context);
            ~PackageSection();

            void Update() override;
            void HandleSectionResize() override;
    };

}
#pragma once

#include "sections/section.hpp"

namespace sections {

    class PackageSection : public Section
    {
        public:
            PackageSection(win32_program::MainWindowContext& context);
            ~PackageSection();

            void Update() override;
            void HandleSectionResize() override;
    };

}
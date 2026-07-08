#pragma once

#include "sections/section.hpp"

namespace sections {

    class LayersSection : public Section
    {
        public:
            LayersSection(win32_program::Win32Context& context);
            ~LayersSection();

            void Update() override;
            void HandleSectionResize() override;
    };

}
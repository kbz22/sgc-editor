#pragma once

#include "sections/section.hpp"

namespace sections {

    class LayersSection : public Section
    {
        public:
            LayersSection(win32_program::MainWindowContext& context);
            ~LayersSection();

            void Update() override;
            void HandleSectionResize() override;
    };

}
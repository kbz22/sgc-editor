#pragma once

#include "sections/section.hpp"

namespace program {
    struct ProgramContext;
}

namespace sections {

    class PackageSection : public Section
    {
        public:
            PackageSection(program::ProgramContext& programContext);
            ~PackageSection();

            void Update() override;
            void HandleSectionResize() override;
            void Refresh(program::ProgramContext& programContext) override;
    };

}
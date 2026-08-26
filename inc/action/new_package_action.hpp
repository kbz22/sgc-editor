#pragma once

#include "action/action.hpp"

namespace action {

    class NewPackageAction : public Action
    {
        public:
            NewPackageAction();            

            void Execute(program::ProgramContext &programContext) override;
    };

}
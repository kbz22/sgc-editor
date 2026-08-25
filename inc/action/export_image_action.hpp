#pragma once

#include "action/action.hpp"

namespace action {

    class ExportImageAction : public Action
    {
        public:
            ExportImageAction();
            
            void Execute(program::ProgramContext& context) override;
    };

}
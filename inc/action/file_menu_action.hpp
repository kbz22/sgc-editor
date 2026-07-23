#pragma once

#include "action/popup_menu_action.hpp"

namespace action {

    class FileMenuAction : public PopupMenuAction
    {
        public:
            FileMenuAction();
            void Execute(program::ProgramContext& context) override;
    };    

}
#pragma once

#include "action/action.hpp"
#include <vector>
#include <memory>
#include <windows.h>

namespace program {
    struct ProgramContext;
}

namespace action {

    class PopupMenuAction : public Action
    {
        private:
            std::vector<Action*> m_popupMenuItems{};

        protected:
            HMENU m_hMenu = HMENU();
            int m_menuId = -1;

        public:
            PopupMenuAction();

            void Execute(program::ProgramContext &context) override;
            bool IsPopup() const override;
            std::optional<std::vector<Action*>> GetPopupMenuItems() const override;

            void SetItems(std::vector<Action*> items);
            void BuildMenu(program::ProgramContext &context);
            HMENU GetHMenu() const;

            void RefreshMenu();
    };

}
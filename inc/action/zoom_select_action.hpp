#pragma once

#include "action/widget_action.hpp"
#include "win32_models/zoom_combo_box.hpp"
#include <memory>

namespace action {

    class ZoomSelectAction : public WidgetAction 
    {
        private:
            std::unique_ptr<win32_models::ZoomComboBox> m_zoomComboBox = nullptr;

        public:
            ZoomSelectAction();
            ~ZoomSelectAction() = default;

            void Execute(program::ProgramContext& context) override;

            void BuildWidget(HWND parent, program::ProgramContext& context) override;
            int GetControlWidth() const override;
            HWND GetHWND() const override;
            win32_models::IWidget* GetWidget() const override;
    };

}
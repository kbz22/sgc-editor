#pragma once

#include "action/action.hpp"
#include "win32_models/iwidget.hpp"
#include <windows.h>

namespace action {

    class WidgetAction : public Action
    {
        public:
            bool IsWidget() const override;
           
            virtual void BuildWidget(HWND parent, program::ProgramContext& context) = 0;            
            virtual int GetControlWidth() const = 0;
            virtual HWND GetHWND() const = 0;            
            virtual win32_models::IWidget* GetWidget() const = 0;
    };

}
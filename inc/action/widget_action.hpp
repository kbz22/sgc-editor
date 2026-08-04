#pragma once

#include "action/action.hpp"
#include <windows.h>

namespace action {

    class WidgetAction : public Action
    {
        private:
            HWND m_controlHandle = nullptr;

        protected:
            void SetControlHandle(HWND hwnd);

        public:        
            bool IsWidget() const override;

            virtual HWND CreateControl(HWND parent, HINSTANCE hInstance) = 0;
            virtual int GetControlWidth() const = 0;

            HWND GetHWND() const;            
    };

}
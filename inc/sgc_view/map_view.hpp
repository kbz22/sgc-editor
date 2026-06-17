#pragma once

#include "sgc_view/sgc_view.hpp"

namespace sgc_view
{
    class MapView : public SgcView
    {
        protected:
            // LRESULT HandleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;

        public:
            MapView(HWND hwnd);
            ~MapView();

            // void Render() override;
    };
}
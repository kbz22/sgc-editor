#pragma once

#include "sgc_view/sgc_view.hpp"
#include "sgc/types.hpp"

namespace sgc_view
{
    using namespace sgc;

    class MapView : public SgcView
    {
        private:
            math::vec2 m_tilePosition = math::vec2{ 0, 0 };

        protected:
            LRESULT HandleMessages(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;

        public:
            MapView(HWND hwnd);
            ~MapView();

            void SetTile(int tileX, int tileY);

            // void Render() override;
    };
}
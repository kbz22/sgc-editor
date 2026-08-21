#pragma once

#include <sgc/math/vector.hpp>
#include <sgc/graphics/line.hpp>
#include <sgc/graphics/idrawable.hpp>
#include <sgc/graphics/color.hpp>

namespace sgc::graphics {

    class MarchingAntsRectangle : public IDrawable {

        private:
            sgc::math::fvec2 m_position;
            sgc::math::fvec2 m_size;
            float m_segmentLength = 4.0f;
            float m_offset = 0.0f;            
            sgc::graphics::color m_color1{ 133, 196, 255, 255 };
            sgc::graphics::color m_color2{ 101, 162, 220, 255 };

        public:
            MarchingAntsRectangle(sgc::math::fvec2 position, sgc::math::fvec2 size);

            void SetPosition(sgc::math::fvec2 position);
            void SetSize(sgc::math::fvec2 size);
            void SetColor(sgc::graphics::color color1, sgc::graphics::color color2);
            void SetOffset(float offset);

            sgc::math::fvec2 GetPosition() const;
            sgc::math::fvec2 GetSize() const;
            float GetSegmentLength() const;

            void Draw(const RenderContext& context) override;

    };

}

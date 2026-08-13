#include "sgc_extension/marching_ants_rectangle.hpp"
#include "sgc/coordinates/screenworld.hpp"
#include <cmath>

sgc::graphics::MarchingAntsRectangle::MarchingAntsRectangle(sgc::math::fvec2 position, sgc::math::fvec2 size) :
    m_position(position),
    m_size(size)
{}

void sgc::graphics::MarchingAntsRectangle::SetPosition(sgc::math::fvec2 position) 
{
    m_position = position;
}

void sgc::graphics::MarchingAntsRectangle::SetSize(sgc::math::fvec2 size) 
{
    m_size = size;
}

void sgc::graphics::MarchingAntsRectangle::SetColor(sgc::graphics::color color1, sgc::graphics::color color2) 
{
    m_color1 = color1;
    m_color2 = color2;
}

void sgc::graphics::MarchingAntsRectangle::SetOffset(float offset) 
{
    m_offset = std::fmod(offset, 2 * m_segmentLength);
}

sgc::math::fvec2 sgc::graphics::MarchingAntsRectangle::GetPosition() const 
{
    return m_position;
}

sgc::math::fvec2 sgc::graphics::MarchingAntsRectangle::GetSize() const 
{
    return m_size;
}

void sgc::graphics::MarchingAntsRectangle::Draw(const RenderContext& context) 
{    
    constexpr int numPoints = 4;
    sgc::math::fvec2 points[numPoints] = {
        sgc::coordinates::WorldToScreen(m_position, context.view),
        sgc::coordinates::WorldToScreen(m_position + sgc::math::fvec2{ m_size.x, 0.0f }, context.view),
        sgc::coordinates::WorldToScreen(m_position + m_size, context.view),
        sgc::coordinates::WorldToScreen(m_position + sgc::math::fvec2{ 0.0f, m_size.y }, context.view)
    };

    auto drawStrippedLine = [&](sgc::math::fvec2 start, sgc::math::fvec2 end, sgc::graphics::color color1, sgc::graphics::color color2) {
        SDL_SetRenderDrawColor(
            context.renderer,
            color1.r,
            color1.g,
            color1.b,
            color1.a
        );

        SDL_RenderLine(
            context.renderer,
            start.x,
            start.y,
            end.x,
            end.y
        );

        SDL_SetRenderDrawColor(
            context.renderer,
            color2.r,
            color2.g,
            color2.b,
            color2.a
        );

        auto direction = sgc::math::Normalize(end - start);
        auto lineLength = sgc::math::VectorLength(end - start);
        auto segment = direction * m_segmentLength;

        for(float d = m_offset; d < lineLength; d += 2 * m_segmentLength)
        {
            float dashStart = d;
            float dashEnd = std::min(d + m_segmentLength, lineLength);

            auto p1 = start + direction * dashStart;
            auto p2 = start + direction * dashEnd;

            SDL_RenderLine(
                context.renderer,
                p1.x,
                p1.y,
                p2.x,
                p2.y
            );
        }
    };

    for(int i = 0; i < numPoints; ++i) {
        auto start = points[i];
        auto end = points[(i + 1) % numPoints];

        drawStrippedLine(start, end, m_color1, m_color2);
    }
}
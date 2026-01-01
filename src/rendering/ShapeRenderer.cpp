#include "ShapeRenderer.h"
#include <cmath>

void ShapeRenderer::drawThickLine(
    SDL_Renderer* renderer,
    const Vector2D& start,
    const Vector2D& end,
    const SDL_Color& color,
    int thickness)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // Calculate direction and perpendicular
    Vector2D dir = (end - start).normalized();
    Vector2D perp(-dir.y, dir.x);  // 90° rotation

    // Draw multiple parallel lines for thickness
    int halfThickness = thickness / 2;
    for (int t = -halfThickness; t <= halfThickness; ++t) {
        Vector2D offset = perp * static_cast<float>(t);
        SDL_RenderDrawLine(
            renderer,
            static_cast<int>(start.x + offset.x),
            static_cast<int>(start.y + offset.y),
            static_cast<int>(end.x + offset.x),
            static_cast<int>(end.y + offset.y)
        );
    }
}

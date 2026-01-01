#pragma once

#include <SDL2/SDL.h>
#include "../math/Vector2D.h"

class ShapeRenderer {
public:
    // Draw a thick line between two points
    static void drawThickLine(
        SDL_Renderer* renderer,
        const Vector2D& start,
        const Vector2D& end,
        const SDL_Color& color,
        int thickness
    );
};

#include "Ball.h"
#include "../math/MathUtils.h"

uint32_t Ball::nextId = 0;

Ball::Ball(const Vector2D& position, const Vector2D& velocity,
           float radius, const SDL_Color& color)
    : position(position)
    , velocity(velocity)
    , radius(radius)
    , mass(0.0f)
    , color(color)
    , id(nextId++)
{
    calculateMass();
}

void Ball::update(float deltaTime) {
    // Update position based on velocity
    position += velocity * deltaTime;
}

void Ball::applyGravity(float gravity, float deltaTime) {
    // Gravity acts downward (positive Y direction)
    velocity.y += gravity * deltaTime;
}

bool Ball::isOffScreen(float screenWidth, float screenHeight) const {
    // Check if ball has completely exited through any edge
    // Top edge (top of ball above screen)
    if (position.y - radius < 0) return true;

    // Bottom edge (bottom of ball below screen)
    if (position.y + radius > screenHeight) return true;

    // Left edge (right side of ball left of screen)
    if (position.x + radius < 0) return true;

    // Right edge (left side of ball right of screen)
    if (position.x - radius > screenWidth) return true;

    return false;
}

void Ball::calculateMass() {
    // Mass proportional to area: m = π * r²
    mass = MathUtils::PI * radius * radius;
}

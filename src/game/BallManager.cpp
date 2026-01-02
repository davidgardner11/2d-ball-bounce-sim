#include "BallManager.h"
#include "../core/Config.h"
#include "../math/MathUtils.h"
#include <cstdlib>
#include <ctime>

BallManager::BallManager(const Vector2D& spawnCenter, float ballRadius)
    : spawnCenter(spawnCenter)
    , ballRadius(ballRadius)
    , pendingRespawnCount(0)
{
    // Seed random number generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

void BallManager::spawnInitialBall() {
    Ball ball = createRandomBall(spawnCenter);
    balls.push_back(ball);
}

void BallManager::update(float screenWidth, float screenHeight, int respawnCount) {
    // Count how many balls are off-screen
    size_t offScreenCount = 0;

    // Remove balls that exited through any edge
    auto it = balls.begin();
    while (it != balls.end()) {
        if (it->isOffScreen(screenWidth, screenHeight)) {
            ++offScreenCount;
            it = balls.erase(it);
        } else {
            ++it;
        }
    }

    // Add to pending respawn queue
    if (offScreenCount > 0) {
        pendingRespawnCount += offScreenCount * respawnCount;
    }

    // Try to spawn pending balls (only if spawn point is clear)
    if (pendingRespawnCount > 0 && !wouldCollideWithBalls(spawnCenter)) {
        // Spawn one ball at a time when space is available
        Ball ball = createRandomBall(spawnCenter);
        balls.push_back(ball);
        pendingRespawnCount--;
    }
}

Ball BallManager::createRandomBall(const Vector2D& position) {
    Vector2D velocity = getRandomVelocity();
    SDL_Color color = getRandomColor();
    return Ball(position, velocity, ballRadius, color);
}

Vector2D BallManager::getRandomVelocity() const {
    // Random angle (0 to 2π)
    float angle = MathUtils::randomRange(0.0f, MathUtils::TWO_PI);

    // Random speed
    float speed = MathUtils::randomRange(Config::BALL_MIN_VELOCITY, Config::BALL_MAX_VELOCITY);

    // Create velocity vector
    return Vector2D::fromAngle(angle, speed);
}

SDL_Color BallManager::getRandomColor() const {
    // Generate vibrant random colors
    Uint8 r = static_cast<Uint8>(MathUtils::randomRangeInt(100, 255));
    Uint8 g = static_cast<Uint8>(MathUtils::randomRangeInt(100, 255));
    Uint8 b = static_cast<Uint8>(MathUtils::randomRangeInt(100, 255));

    return SDL_Color{r, g, b, 255};
}

void BallManager::removeOffScreenBalls(float screenHeight) {
    // This method is unused - ball removal is handled in update()
    // Kept for compatibility but updated signature
    (void)screenHeight; // Suppress unused warning
}

bool BallManager::wouldCollideWithBalls(const Vector2D& position) const {
    // Check if spawning a ball at this position would collide with any existing ball
    // Use a safety margin of 2x the combined radii to ensure adequate spacing
    for (const auto& ball : balls) {
        float distance = position.distance(ball.position);
        float minDistance = ballRadius + ball.radius;
        float safeDistance = minDistance * 2.0f;  // Require 2x spacing

        // If distance is less than safe distance, position is not clear
        if (distance < safeDistance) {
            return true;
        }
    }
    return false;
}

void BallManager::spawnReplacementBalls(size_t count) {
    for (size_t i = 0; i < count; ++i) {
        Ball ball = createRandomBall(spawnCenter);
        balls.push_back(ball);
    }
}

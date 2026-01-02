#pragma once

#include "../entities/Ball.h"
#include "../math/Vector2D.h"
#include <vector>

class BallManager {
public:
    BallManager(const Vector2D& spawnCenter, float ballRadius);

    // Initialize with first ball
    void spawnInitialBall();

    // Update: remove off-screen balls and spawn replacements
    void update(float screenWidth, float screenHeight, int respawnCount = 2);

    // Access balls
    std::vector<Ball>& getBalls() { return balls; }
    const std::vector<Ball>& getBalls() const { return balls; }

    // Stats
    size_t getBallCount() const { return balls.size(); }
    size_t getPendingRespawnCount() const { return pendingRespawnCount; }

    // Configuration
    void setBallRadius(float radius) { ballRadius = radius; }

private:
    std::vector<Ball> balls;
    Vector2D spawnCenter;
    float ballRadius;
    size_t pendingRespawnCount;

    // Spawning helpers
    Ball createRandomBall(const Vector2D& position);
    Vector2D getRandomVelocity() const;
    SDL_Color getRandomColor() const;

    // Check if a position would collide with existing balls
    bool wouldCollideWithBalls(const Vector2D& position) const;

    // Remove balls that fell off screen
    void removeOffScreenBalls(float screenHeight);

    // Spawn 2 balls for each removed ball
    void spawnReplacementBalls(size_t count);
};

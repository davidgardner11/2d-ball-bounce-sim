#include "GameState.h"
#include "../core/Config.h"

GameState::GameState()
    : ballManager(
        Vector2D(Config::CONTAINER_CENTER_X, Config::CONTAINER_CENTER_Y),
        Config::BALL_RADIUS
    )
    , container(
        Vector2D(Config::CONTAINER_CENTER_X, Config::CONTAINER_CENTER_Y),
        Config::CONTAINER_RADIUS * 2.0f,  // Use diameter as side length (600px)
        0.25f,                             // Gap is 25% of side length
        0                                  // Gap on side 0 (right side in local coords)
    )
    , physics(Config::GRAVITY)
{
}

void GameState::initialize() {
    // Spawn the initial ball
    ballManager.spawnInitialBall();
}

void GameState::update(float deltaTime, float restitution, int respawnCount) {
    // Update container rotation
    container.update(deltaTime);

    // Update physics simulation
    physics.update(ballManager.getBalls(), container, deltaTime, restitution);

    // Update ball manager (remove off-screen balls, spawn replacements)
    ballManager.update(
        static_cast<float>(Config::WINDOW_WIDTH),
        static_cast<float>(Config::WINDOW_HEIGHT),
        respawnCount
    );
}

size_t GameState::getBallCount() const {
    return ballManager.getBallCount();
}

#include "GameState.h"
#include "../core/Config.h"

GameState::GameState()
    : ballManager(
        Vector2D(Config::CONTAINER_CENTER_X, Config::CONTAINER_CENTER_Y),
        Config::BALL_RADIUS
    )
    , container(
        Vector2D(Config::CONTAINER_CENTER_X, Config::CONTAINER_CENTER_Y),
        Config::CONTAINER_RADIUS,
        Config::CONTAINER_GAP_PERCENT * 360.0f  // Convert to degrees
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

size_t GameState::getPendingRespawnCount() const {
    return ballManager.getPendingRespawnCount();
}

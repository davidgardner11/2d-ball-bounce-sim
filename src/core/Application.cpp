#include "Application.h"
#include "Config.h"
#include "../math/MathUtils.h"
#include <iostream>

Application::Application()
    : renderer(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT, Config::WINDOW_TITLE)
    , bouncinessSlider(Config::SLIDER_X, Config::SLIDER_Y, Config::SLIDER_WIDTH, Config::SLIDER_HEIGHT, 0.95f, 1.05f, Config::RESTITUTION)
    , ballSizeSlider(Config::SIZE_SLIDER_X, Config::SIZE_SLIDER_Y, Config::SIZE_SLIDER_WIDTH, Config::SIZE_SLIDER_HEIGHT, 5.0f, 25.0f, Config::BALL_RADIUS)
    , holeSizeSlider(Config::HOLE_SLIDER_X, Config::HOLE_SLIDER_Y, Config::HOLE_SLIDER_WIDTH, Config::HOLE_SLIDER_HEIGHT, 0.0f, 180.0f, Config::CONTAINER_GAP_PERCENT * 360.0f)
    , respawnCountSlider(Config::RESPAWN_SLIDER_X, Config::RESPAWN_SLIDER_Y, Config::RESPAWN_SLIDER_WIDTH, Config::RESPAWN_SLIDER_HEIGHT, 0.1f, 10.0f, 2.0f)
    , gravitySlider(Config::GRAVITY_SLIDER_X, Config::GRAVITY_SLIDER_Y, Config::GRAVITY_SLIDER_WIDTH, Config::GRAVITY_SLIDER_HEIGHT, 0.0f, 20.0f, 9.8f)
    , diameterSlider(Config::DIAMETER_SLIDER_X, Config::DIAMETER_SLIDER_Y, Config::DIAMETER_SLIDER_WIDTH, Config::DIAMETER_SLIDER_HEIGHT, 200.0f, 800.0f, Config::CONTAINER_RADIUS * 2.0f)
    , resetButton(Config::RESET_BUTTON_X, Config::RESET_BUTTON_Y, Config::RESET_BUTTON_WIDTH, Config::RESET_BUTTON_HEIGHT, "Reset")
    , pauseButton(Config::PAUSE_BUTTON_X, Config::PAUSE_BUTTON_Y, Config::PAUSE_BUTTON_WIDTH, Config::PAUSE_BUTTON_HEIGHT, "Pause")
    , restitution(Config::RESTITUTION)
    , ballRadius(Config::BALL_RADIUS)
    , holeSize(Config::CONTAINER_GAP_PERCENT * 360.0f)
    , respawnRate(2.0f)
    , gravity(9.8f)
    , containerDiameter(Config::CONTAINER_RADIUS * 2.0f)
    , running(false)
    , paused(false)
    , accumulator(0.0f)
{
    // Set up reset button callback
    resetButton.setOnClick([this]() {
        resetSimulation();
    });

    // Set up pause button callback
    pauseButton.setOnClick([this]() {
        paused = !paused;
    });
}

Application::~Application() {
    cleanup();
}

bool Application::initialize() {
    // Initialize renderer
    if (!renderer.initialize()) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return false;
    }

    // Initialize circle renderer
    circleRenderer.initialize(renderer.getSDLRenderer());

    // Initialize text renderer
    if (!textRenderer.initialize()) {
        std::cerr << "Failed to initialize text renderer" << std::endl;
        return false;
    }

    // Initialize game state
    gameState.initialize();

    running = true;
    return true;
}

void Application::run() {
    while (running) {
        time.tick();
        float frameTime = time.getDeltaTime();

        // Prevent spiral of death
        if (frameTime > 0.25f) {
            frameTime = 0.25f;
        }

        accumulator += frameTime;

        // Handle events
        handleEvents();

        // Fixed timestep updates
        int steps = 0;
        while (accumulator >= Config::FIXED_TIMESTEP && steps < Config::MAX_PHYSICS_STEPS) {
            update(Config::FIXED_TIMESTEP);
            accumulator -= Config::FIXED_TIMESTEP;
            steps++;
        }

        // Render
        render();
    }
}

void Application::cleanup() {
    circleRenderer.cleanup();
    textRenderer.cleanup();
    renderer.cleanup();
}

void Application::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        } else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            bouncinessSlider.handleMouseDown(event.button.x, event.button.y);
            ballSizeSlider.handleMouseDown(event.button.x, event.button.y);
            holeSizeSlider.handleMouseDown(event.button.x, event.button.y);
            respawnCountSlider.handleMouseDown(event.button.x, event.button.y);
            gravitySlider.handleMouseDown(event.button.x, event.button.y);
            diameterSlider.handleMouseDown(event.button.x, event.button.y);
            resetButton.handleMouseDown(event.button.x, event.button.y);
            pauseButton.handleMouseDown(event.button.x, event.button.y);
        } else if (event.type == SDL_MOUSEBUTTONUP) {
            bouncinessSlider.handleMouseUp();
            ballSizeSlider.handleMouseUp();
            holeSizeSlider.handleMouseUp();
            respawnCountSlider.handleMouseUp();
            gravitySlider.handleMouseUp();
            diameterSlider.handleMouseUp();
            resetButton.handleMouseUp(event.button.x, event.button.y);
            pauseButton.handleMouseUp(event.button.x, event.button.y);
        } else if (event.type == SDL_MOUSEMOTION) {
            bouncinessSlider.handleMouseMove(event.motion.x, event.motion.y);
            ballSizeSlider.handleMouseMove(event.motion.x, event.motion.y);
            holeSizeSlider.handleMouseMove(event.motion.x, event.motion.y);
            respawnCountSlider.handleMouseMove(event.motion.x, event.motion.y);
            gravitySlider.handleMouseMove(event.motion.x, event.motion.y);
            diameterSlider.handleMouseMove(event.motion.x, event.motion.y);
            resetButton.handleMouseMove(event.motion.x, event.motion.y);
            pauseButton.handleMouseMove(event.motion.x, event.motion.y);
            // Update values from sliders
            restitution = bouncinessSlider.getValue();
            ballRadius = ballSizeSlider.getValue();
            holeSize = holeSizeSlider.getValue();
            respawnRate = respawnCountSlider.getValue();
            gravity = gravitySlider.getValue();
            containerDiameter = diameterSlider.getValue();
        }
    }
}

void Application::update(float deltaTime) {
    // Skip update if paused
    if (paused) {
        return;
    }

    // Update configurable parameters
    gameState.getBallManager().setBallRadius(ballRadius);
    gameState.getContainer().setGapAngleDegrees(holeSize);
    gameState.getContainer().setRadius(containerDiameter / 2.0f);
    gameState.getPhysics().setGravity(gravity * 100.0f); // Convert m/s² to px/s²

    // Update game state with respawn rate (BallManager handles queuing and safe spawning)
    int respawnCount = static_cast<int>(respawnRate);
    gameState.update(deltaTime, restitution, respawnCount);

    // Ensure at least one ball exists to keep simulation running
    if (gameState.getBallCount() == 0 && gameState.getPendingRespawnCount() == 0) {
        gameState.getBallManager().spawnInitialBall();
    }
}

void Application::render() {
    // Clear screen
    renderer.clear(Config::BACKGROUND_COLOR);

    // Render game objects
    renderContainer();
    renderBalls();
    renderUI();

    // Present
    renderer.endFrame();
}

void Application::renderContainer() {
    const Container& container = gameState.getContainer();

    Vector2D center = container.getCenter();
    float radius = container.getRadius();
    float gapStart = container.getGapStartAngle();
    float gapEnd = container.getGapEndAngle();

    // Draw the container as an arc (excluding the gap)
    // We need to draw from gapEnd to gapStart (the complement of the gap)
    float arcStart = gapEnd;
    float arcEnd = gapStart + MathUtils::TWO_PI;

    circleRenderer.drawArc(
        renderer.getSDLRenderer(),
        center,
        radius,
        arcStart,
        arcEnd,
        Config::CONTAINER_COLOR,
        3  // thickness
    );
}

void Application::renderBalls() {
    const std::vector<Ball>& balls = gameState.getBallManager().getBalls();

    for (const Ball& ball : balls) {
        circleRenderer.drawFilledCircleFast(
            renderer.getSDLRenderer(),
            ball.position,
            ball.radius,
            ball.color
        );
    }
}

void Application::renderUI() {
    // Render reset button
    resetButton.render(renderer.getSDLRenderer());
    textRenderer.renderText(
        renderer.getSDLRenderer(),
        "Reset",
        Config::RESET_BUTTON_X + 25,
        Config::RESET_BUTTON_Y + 7,
        Config::TEXT_COLOR
    );

    // Render pause button
    pauseButton.render(renderer.getSDLRenderer());
    textRenderer.renderText(
        renderer.getSDLRenderer(),
        paused ? "Resume" : "Pause",
        Config::PAUSE_BUTTON_X + (paused ? 18 : 22),
        Config::PAUSE_BUTTON_Y + 7,
        Config::TEXT_COLOR
    );

    // Render FPS (cached)
    textRenderer.renderFPSCached(
        renderer.getSDLRenderer(),
        time.getFPS(),
        Config::FPS_DISPLAY_X,
        Config::FPS_DISPLAY_Y
    );

    // Render ball count (cached)
    textRenderer.renderBallCountCached(
        renderer.getSDLRenderer(),
        gameState.getBallCount(),
        Config::BALL_COUNT_X,
        Config::BALL_COUNT_Y
    );

    // Render timer (cached)
    textRenderer.renderTimerCached(
        renderer.getSDLRenderer(),
        time.getElapsedTime(),
        Config::TIMER_DISPLAY_X,
        Config::TIMER_DISPLAY_Y
    );

    // Render pending respawn count (cached)
    textRenderer.renderPendingRespawnCached(
        renderer.getSDLRenderer(),
        gameState.getPendingRespawnCount(),
        Config::PENDING_RESPAWN_X,
        Config::PENDING_RESPAWN_Y
    );

    // Render bounciness slider
    bouncinessSlider.render(renderer.getSDLRenderer(), "Bounciness");

    // Render slider label and value
    char bouncinessLabel[64];
    snprintf(bouncinessLabel, sizeof(bouncinessLabel), "Bounciness: %.0f%%", restitution * 100.0f);
    textRenderer.renderText(
        renderer.getSDLRenderer(),
        bouncinessLabel,
        Config::SLIDER_X,
        Config::SLIDER_Y - 25,
        Config::TEXT_COLOR
    );

    // Render ball size slider
    ballSizeSlider.render(renderer.getSDLRenderer(), "Ball Size");

    // Render ball size label and value
    char sizeLabel[64];
    snprintf(sizeLabel, sizeof(sizeLabel), "Ball Size: %.0fpx", ballRadius * 2.0f);
    textRenderer.renderText(
        renderer.getSDLRenderer(),
        sizeLabel,
        Config::SIZE_SLIDER_X,
        Config::SIZE_SLIDER_Y - 25,
        Config::TEXT_COLOR
    );

    // Render hole size slider
    holeSizeSlider.render(renderer.getSDLRenderer(), "Hole Size");

    // Render hole size label and value
    char holeLabel[64];
    snprintf(holeLabel, sizeof(holeLabel), "Hole Size: %.0f deg", holeSize);
    textRenderer.renderText(
        renderer.getSDLRenderer(),
        holeLabel,
        Config::HOLE_SLIDER_X,
        Config::HOLE_SLIDER_Y - 25,
        Config::TEXT_COLOR
    );

    // Render respawn rate slider
    respawnCountSlider.render(renderer.getSDLRenderer(), "Respawn Rate");

    // Render respawn rate label and value
    char respawnLabel[64];
    if (respawnRate >= 1.0f) {
        snprintf(respawnLabel, sizeof(respawnLabel), "Respawn: %dx", static_cast<int>(respawnRate));
    } else {
        // For rates < 1, show as "2:1" ratio format
        float ratio = 1.0f / respawnRate;
        snprintf(respawnLabel, sizeof(respawnLabel), "Respawn: %.0f:1 balls", ratio);
    }
    textRenderer.renderText(
        renderer.getSDLRenderer(),
        respawnLabel,
        Config::RESPAWN_SLIDER_X,
        Config::RESPAWN_SLIDER_Y - 25,
        Config::TEXT_COLOR
    );

    // Render gravity slider
    gravitySlider.render(renderer.getSDLRenderer(), "Gravity");

    // Render gravity label and value
    char gravityLabel[64];
    snprintf(gravityLabel, sizeof(gravityLabel), "Gravity: %.1f m/s^2", gravity);
    textRenderer.renderText(
        renderer.getSDLRenderer(),
        gravityLabel,
        Config::GRAVITY_SLIDER_X,
        Config::GRAVITY_SLIDER_Y - 25,
        Config::TEXT_COLOR
    );

    // Render container diameter slider
    diameterSlider.render(renderer.getSDLRenderer(), "Container Diameter");

    // Render diameter label and value
    char diameterLabel[64];
    snprintf(diameterLabel, sizeof(diameterLabel), "Cont Diameter: %.0fpx", containerDiameter);
    textRenderer.renderText(
        renderer.getSDLRenderer(),
        diameterLabel,
        Config::DIAMETER_SLIDER_X,
        Config::DIAMETER_SLIDER_Y - 25,
        Config::TEXT_COLOR
    );
}

void Application::resetSimulation() {
    // Clear all balls and reset to initial state
    gameState.getBallManager().getBalls().clear();
    gameState.initialize();

    // Reset timer
    time = Time();
}

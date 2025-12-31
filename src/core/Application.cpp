#include "Application.h"
#include "Config.h"
#include "../math/MathUtils.h"
#include <iostream>

Application::Application()
    : renderer(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT, Config::WINDOW_TITLE)
    , running(false)
    , accumulator(0.0f)
{
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
        }
    }
}

void Application::update(float deltaTime) {
    gameState.update(deltaTime);
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
}

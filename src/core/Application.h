#pragma once

#include "../rendering/Renderer.h"
#include "../rendering/CircleRenderer.h"
#include "../rendering/TextRenderer.h"
#include "../game/GameState.h"
#include "../ui/Slider.h"
#include "../ui/Button.h"
#include "Time.h"

class Application {
public:
    Application();
    ~Application();

    bool initialize();
    void run();
    void cleanup();

private:
    // Core systems
    Renderer renderer;
    GameState gameState;
    Time time;
    CircleRenderer circleRenderer;
    TextRenderer textRenderer;

    // UI elements
    Slider bouncinessSlider;
    Slider ballSizeSlider;
    Slider holeSizeSlider;
    Slider respawnCountSlider;
    Slider gravitySlider;
    Slider diameterSlider;
    Button resetButton;
    Button pauseButton;
    float restitution;
    float ballRadius;
    float holeSize;
    float respawnRate;
    float gravity;
    float containerDiameter;

    bool running;
    bool paused;
    float accumulator;  // For fixed timestep

    // Game loop methods
    void handleEvents();
    void update(float deltaTime);
    void render();

    // Rendering helpers
    void renderContainer();
    void renderBalls();
    void renderUI();

    // Reset functionality
    void resetSimulation();
};

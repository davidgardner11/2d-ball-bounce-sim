#pragma once

#include <SDL2/SDL.h>

namespace Config {
    // Window settings
    constexpr int WINDOW_WIDTH = 1024;
    constexpr int WINDOW_HEIGHT = 768;
    constexpr const char* WINDOW_TITLE = "Ball Bouncing Simulator";

    // Container settings
    constexpr float CONTAINER_RADIUS = 300.0f;  // 600px diameter
    constexpr float CONTAINER_GAP_PERCENT = 0.125f;  // 12.5% of circumference (5% * 2.5)
    constexpr float CONTAINER_ROTATION_PERIOD = 10.0f;  // 10 seconds for full rotation
    constexpr float CONTAINER_ROTATION_SPEED = 36.0f;  // degrees per second (360/10)

    // Container position (center of window)
    constexpr float CONTAINER_CENTER_X = WINDOW_WIDTH / 2.0f;
    constexpr float CONTAINER_CENTER_Y = WINDOW_HEIGHT / 2.0f;

    // Ball settings
    // constexpr float BALL_RADIUS = 12.5f;  // 25px diameter
    constexpr float BALL_RADIUS = 7.5f;  // 15px diameter
    constexpr float BALL_MIN_VELOCITY = 50.0f;   // pixels/second
    constexpr float BALL_MAX_VELOCITY = 200.0f;  // pixels/second

    // Physics settings
    constexpr float GRAVITY = 9.8f * 100.0f;  // 980 px/s² (9.8 m/s² scaled for pixels)
    constexpr float RESTITUTION = 1.0f;  // 100% bounce (perfectly elastic)

    // Simulation settings
    constexpr float FIXED_TIMESTEP = 1.0f / 120.0f;  // 120Hz physics updates
    constexpr int MAX_PHYSICS_STEPS = 5;  // Prevent spiral of death

    // UI settings
    constexpr int FPS_DISPLAY_X = 10;
    constexpr int FPS_DISPLAY_Y = 10;
    constexpr int BALL_COUNT_X = 10;
    constexpr int BALL_COUNT_Y = 40;
    constexpr int TIMER_DISPLAY_X = 10;
    constexpr int TIMER_DISPLAY_Y = 70;
    constexpr int UI_FONT_SIZE = 20;

    // Slider settings (all shifted down by 50px)
    constexpr int SLIDER_X = WINDOW_WIDTH - 220;
    constexpr int SLIDER_Y = 70;
    constexpr int SLIDER_WIDTH = 200;
    constexpr int SLIDER_HEIGHT = 20;

    // Ball size slider settings
    constexpr int SIZE_SLIDER_X = WINDOW_WIDTH - 220;
    constexpr int SIZE_SLIDER_Y = 120;
    constexpr int SIZE_SLIDER_WIDTH = 200;
    constexpr int SIZE_SLIDER_HEIGHT = 20;

    // Container hole size slider settings
    constexpr int HOLE_SLIDER_X = WINDOW_WIDTH - 220;
    constexpr int HOLE_SLIDER_Y = 170;
    constexpr int HOLE_SLIDER_WIDTH = 200;
    constexpr int HOLE_SLIDER_HEIGHT = 20;

    // Ball respawn count slider settings
    constexpr int RESPAWN_SLIDER_X = WINDOW_WIDTH - 220;
    constexpr int RESPAWN_SLIDER_Y = 220;
    constexpr int RESPAWN_SLIDER_WIDTH = 200;
    constexpr int RESPAWN_SLIDER_HEIGHT = 20;

    // Gravity slider settings
    constexpr int GRAVITY_SLIDER_X = WINDOW_WIDTH - 220;
    constexpr int GRAVITY_SLIDER_Y = 270;
    constexpr int GRAVITY_SLIDER_WIDTH = 200;
    constexpr int GRAVITY_SLIDER_HEIGHT = 20;

    // Button settings
    constexpr int RESET_BUTTON_X = 10;
    constexpr int RESET_BUTTON_Y = 100;
    constexpr int RESET_BUTTON_WIDTH = 100;
    constexpr int RESET_BUTTON_HEIGHT = 30;

    constexpr int PAUSE_BUTTON_X = 10;
    constexpr int PAUSE_BUTTON_Y = 140;
    constexpr int PAUSE_BUTTON_WIDTH = 100;
    constexpr int PAUSE_BUTTON_HEIGHT = 30;

    // Container diameter slider settings
    constexpr int DIAMETER_SLIDER_X = WINDOW_WIDTH - 220;
    constexpr int DIAMETER_SLIDER_Y = 320;
    constexpr int DIAMETER_SLIDER_WIDTH = 200;
    constexpr int DIAMETER_SLIDER_HEIGHT = 20;

    // Colors
    const SDL_Color BACKGROUND_COLOR = {20, 20, 30, 255};
    const SDL_Color CONTAINER_COLOR = {200, 200, 200, 255};
    const SDL_Color TEXT_COLOR = {255, 255, 255, 255};
}

#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();

    bool initialize();
    void cleanup();

    // Render text at position
    void renderText(
        SDL_Renderer* renderer,
        const std::string& text,
        int x, int y,
        const SDL_Color& color,
        int fontSize = 24
    );

    // Render FPS counter
    void renderFPS(SDL_Renderer* renderer, float fps, int x, int y);

    // Render ball count
    void renderBallCount(SDL_Renderer* renderer, size_t count, int x, int y);

    // Render elapsed time
    void renderTimer(SDL_Renderer* renderer, float elapsedTime, int x, int y);

    // Cached rendering methods
    void renderFPSCached(SDL_Renderer* renderer, float fps, int x, int y);
    void renderBallCountCached(SDL_Renderer* renderer, size_t count, int x, int y);
    void renderTimerCached(SDL_Renderer* renderer, float elapsedTime, int x, int y);
    void renderPendingRespawnCached(SDL_Renderer* renderer, size_t count, int x, int y);

private:
    TTF_Font* font;
    int fontSize;
    bool initialized;

    // Cached textures
    SDL_Texture* fpsCachedTexture;
    float lastFPS;

    SDL_Texture* ballCountCachedTexture;
    size_t lastBallCount;

    SDL_Texture* timerCachedTexture;
    int lastTimerSeconds;

    SDL_Texture* pendingRespawnCachedTexture;
    size_t lastPendingRespawnCount;

    void renderCachedTexture(
        SDL_Renderer* renderer,
        SDL_Texture** cachedTexture,
        const std::string& text,
        int x, int y
    );
};

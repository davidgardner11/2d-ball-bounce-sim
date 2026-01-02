#include "TextRenderer.h"
#include "../core/Config.h"
#include <iostream>
#include <sstream>
#include <iomanip>

TextRenderer::TextRenderer()
    : font(nullptr)
    , fontSize(Config::UI_FONT_SIZE)
    , initialized(false)
    , fpsCachedTexture(nullptr)
    , lastFPS(-1.0f)
    , ballCountCachedTexture(nullptr)
    , lastBallCount(0)
    , timerCachedTexture(nullptr)
    , lastTimerSeconds(-1)
    , pendingRespawnCachedTexture(nullptr)
    , lastPendingRespawnCount(0)
{
}

TextRenderer::~TextRenderer() {
    cleanup();
}

bool TextRenderer::initialize() {
    if (initialized) {
        return true;
    }

    // Initialize SDL_ttf
    if (TTF_Init() < 0) {
        std::cerr << "SDL_ttf initialization failed: " << TTF_GetError() << std::endl;
        return false;
    }

    // Try to load a system font
    // macOS common font paths
    const char* fontPaths[] = {
        "/System/Library/Fonts/Helvetica.ttc",
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/Library/Fonts/Arial.ttf",
        "/System/Library/Fonts/SFNSDisplay.ttf"
    };

    for (const char* path : fontPaths) {
        font = TTF_OpenFont(path, fontSize);
        if (font) {
            break;
        }
    }

    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        TTF_Quit();
        return false;
    }

    initialized = true;
    return true;
}

void TextRenderer::cleanup() {
    if (fpsCachedTexture) {
        SDL_DestroyTexture(fpsCachedTexture);
        fpsCachedTexture = nullptr;
    }
    if (ballCountCachedTexture) {
        SDL_DestroyTexture(ballCountCachedTexture);
        ballCountCachedTexture = nullptr;
    }
    if (timerCachedTexture) {
        SDL_DestroyTexture(timerCachedTexture);
        timerCachedTexture = nullptr;
    }
    if (pendingRespawnCachedTexture) {
        SDL_DestroyTexture(pendingRespawnCachedTexture);
        pendingRespawnCachedTexture = nullptr;
    }

    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }

    if (initialized) {
        TTF_Quit();
        initialized = false;
    }
}

void TextRenderer::renderText(
    SDL_Renderer* renderer,
    const std::string& text,
    int x, int y,
    const SDL_Color& color,
    int)
{
    if (!initialized || !font) {
        return;
    }

    // Create surface from text
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) {
        return;
    }

    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }

    // Render texture
    SDL_Rect destRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &destRect);

    // Cleanup
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void TextRenderer::renderFPS(SDL_Renderer* renderer, float fps, int x, int y) {
    std::ostringstream oss;
    oss << "FPS: " << std::fixed << std::setprecision(1) << fps;
    renderText(renderer, oss.str(), x, y, Config::TEXT_COLOR);
}

void TextRenderer::renderBallCount(SDL_Renderer* renderer, size_t count, int x, int y) {
    std::ostringstream oss;
    oss << "Balls: " << count;
    renderText(renderer, oss.str(), x, y, Config::TEXT_COLOR);
}

void TextRenderer::renderTimer(SDL_Renderer* renderer, float elapsedTime, int x, int y) {
    // Format time as MM:SS.mm
    int minutes = static_cast<int>(elapsedTime) / 60;
    int seconds = static_cast<int>(elapsedTime) % 60;
    int milliseconds = static_cast<int>((elapsedTime - static_cast<int>(elapsedTime)) * 100);

    std::ostringstream oss;
    oss << "Time: "
        << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << seconds << "."
        << std::setfill('0') << std::setw(2) << milliseconds;
    renderText(renderer, oss.str(), x, y, Config::TEXT_COLOR);
}

void TextRenderer::renderFPSCached(SDL_Renderer* renderer, float fps, int x, int y) {
    // Only update if FPS changed significantly (> 1 FPS)
    if (std::abs(fps - lastFPS) < 1.0f && fpsCachedTexture) {
        SDL_Rect destRect;
        SDL_QueryTexture(fpsCachedTexture, nullptr, nullptr, &destRect.w, &destRect.h);
        destRect.x = x;
        destRect.y = y;
        SDL_RenderCopy(renderer, fpsCachedTexture, nullptr, &destRect);
        return;
    }

    lastFPS = fps;
    std::ostringstream oss;
    oss << "FPS: " << std::fixed << std::setprecision(1) << fps;
    renderCachedTexture(renderer, &fpsCachedTexture, oss.str(), x, y);
}

void TextRenderer::renderBallCountCached(SDL_Renderer* renderer, size_t count, int x, int y) {
    if (count == lastBallCount && ballCountCachedTexture) {
        SDL_Rect destRect;
        SDL_QueryTexture(ballCountCachedTexture, nullptr, nullptr, &destRect.w, &destRect.h);
        destRect.x = x;
        destRect.y = y;
        SDL_RenderCopy(renderer, ballCountCachedTexture, nullptr, &destRect);
        return;
    }

    lastBallCount = count;
    std::ostringstream oss;
    oss << "Balls: " << count;
    renderCachedTexture(renderer, &ballCountCachedTexture, oss.str(), x, y);
}

void TextRenderer::renderTimerCached(SDL_Renderer* renderer, float elapsedTime, int x, int y) {
    int currentSeconds = static_cast<int>(elapsedTime);

    // Update every second
    if (currentSeconds == lastTimerSeconds && timerCachedTexture) {
        SDL_Rect destRect;
        SDL_QueryTexture(timerCachedTexture, nullptr, nullptr, &destRect.w, &destRect.h);
        destRect.x = x;
        destRect.y = y;
        SDL_RenderCopy(renderer, timerCachedTexture, nullptr, &destRect);
        return;
    }

    lastTimerSeconds = currentSeconds;
    int minutes = currentSeconds / 60;
    int seconds = currentSeconds % 60;
    int milliseconds = static_cast<int>((elapsedTime - currentSeconds) * 100);

    std::ostringstream oss;
    oss << "Time: "
        << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << seconds << "."
        << std::setfill('0') << std::setw(2) << milliseconds;
    renderCachedTexture(renderer, &timerCachedTexture, oss.str(), x, y);
}

void TextRenderer::renderPendingRespawnCached(SDL_Renderer* renderer, size_t count, int x, int y) {
    if (count == lastPendingRespawnCount && pendingRespawnCachedTexture) {
        SDL_Rect destRect;
        SDL_QueryTexture(pendingRespawnCachedTexture, nullptr, nullptr, &destRect.w, &destRect.h);
        destRect.x = x;
        destRect.y = y;
        SDL_RenderCopy(renderer, pendingRespawnCachedTexture, nullptr, &destRect);
        return;
    }

    lastPendingRespawnCount = count;
    std::ostringstream oss;
    oss << "Respawn Count: " << count;
    renderCachedTexture(renderer, &pendingRespawnCachedTexture, oss.str(), x, y);
}

void TextRenderer::renderCachedTexture(
    SDL_Renderer* renderer,
    SDL_Texture** cachedTexture,
    const std::string& text,
    int x, int y)
{
    // Destroy old texture
    if (*cachedTexture) {
        SDL_DestroyTexture(*cachedTexture);
    }

    // Create new texture
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), Config::TEXT_COLOR);
    if (!surface) return;

    *cachedTexture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect destRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, *cachedTexture, nullptr, &destRect);

    SDL_FreeSurface(surface);
}

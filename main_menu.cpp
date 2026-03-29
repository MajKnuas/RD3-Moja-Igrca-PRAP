#include "main_menu.h"

static SDL_Texture *createTextTexture(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color, int &outW, int &outH) {
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, 0, color);
    if (!surface) {
        outW = 0;
        outH = 0;
        return nullptr;
    }

    outW = surface->w;
    outH = surface->h;
    SDL_Texture *result = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    return result;
}

static bool pointInRect(float x, float y, const SDL_FRect &rect) {
    return x >= rect.x && x <= (rect.x + rect.w) && y >= rect.y && y <= (rect.y + rect.h);
}

SDL_FRect MainMenu::getStartButtonRect(int width, int height) const {
    const float buttonWidth = static_cast<float>(startLabelWidth);
    const float buttonHeight = static_cast<float>(startLabelHeight);
    const float x = (static_cast<float>(width) - buttonWidth) * 0.5f;
    const float y = static_cast<float>(height) * 0.58f;
    return {x, y, buttonWidth, buttonHeight};
}

SDL_FRect MainMenu::getSettingsButtonRect(int width, int height) const {
    SDL_FRect startRect = getStartButtonRect(width, height);
    const float gap = 18.0f;
    return {startRect.x, startRect.y + startRect.h + gap, static_cast<float>(settingsLabelWidth), static_cast<float>(settingsLabelHeight)};
}

void MainMenu::load(SDL_Renderer *renderer, const char *path) {
    texture = IMG_LoadTexture(renderer, path);

    if (!TTF_Init()) {
        return;
    }

    // font ki ga urporabljamo
    font = TTF_OpenFont("fonts/VCR_OSD_MONO_1.001.ttf", 56.0f);
    if (!font) {
        return;
    }

    SDL_Color white = {255, 255, 255, 255};
    startLabelTexture = createTextTexture(renderer, font, "START", white, startLabelWidth, startLabelHeight);
    settingsLabelTexture = createTextTexture(renderer, font, "SETTINGS", white, settingsLabelWidth, settingsLabelHeight);
}

void MainMenu::render(SDL_Renderer *renderer, int width, int height) const {
    if (texture) {
        SDL_FRect target = {0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)};
        SDL_RenderTexture(renderer, texture, nullptr, &target);
    } else {
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);
    }

    SDL_FRect startRect = getStartButtonRect(width, height);
    SDL_FRect settingsRect = getSettingsButtonRect(width, height);

    if (startLabelTexture) {
        SDL_RenderTexture(renderer, startLabelTexture, nullptr, &startRect);
    }

    if (settingsLabelTexture) {
        SDL_RenderTexture(renderer, settingsLabelTexture, nullptr, &settingsRect);
    }
}

void MainMenu::renderSettings(SDL_Renderer *renderer, int width, int height) const {
    (void)width;
    (void)height;
    SDL_SetRenderDrawColor(renderer, 12, 12, 12, 255);
    SDL_RenderClear(renderer);
}

bool MainMenu::isStartButtonPressed(float mouseX, float mouseY, int width, int height) const {
    return pointInRect(mouseX, mouseY, getStartButtonRect(width, height));
}

bool MainMenu::isSettingsButtonPressed(float mouseX, float mouseY, int width, int height) const {
    return pointInRect(mouseX, mouseY, getSettingsButtonRect(width, height));
}

void MainMenu::cleanup() {
    if (startLabelTexture) {
        SDL_DestroyTexture(startLabelTexture);
        startLabelTexture = nullptr;
    }

    if (settingsLabelTexture) {
        SDL_DestroyTexture(settingsLabelTexture);
        settingsLabelTexture = nullptr;
    }

    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }

    TTF_Quit();

    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

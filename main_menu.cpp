#include "main_menu.h"

bool MainMenu::load(SDL_Renderer *renderer, const char *path) {
    texture = IMG_LoadTexture(renderer, path);
    return texture != nullptr;
}

void MainMenu::render(SDL_Renderer *renderer, int width, int height) const {
    if (!texture) {
        return;
    }

    SDL_FRect target = {0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)};
    SDL_RenderTexture(renderer, texture, nullptr, &target);
}

void MainMenu::cleanup() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

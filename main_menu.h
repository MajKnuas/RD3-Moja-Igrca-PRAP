#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

class MainMenu {
private:
    SDL_Texture *texture = nullptr;
    TTF_Font *font = nullptr;
    SDL_Texture *startLabelTexture = nullptr;
    SDL_Texture *settingsLabelTexture = nullptr;
    int startLabelWidth = 0;
    int startLabelHeight = 0;
    int settingsLabelWidth = 0;
    int settingsLabelHeight = 0;

    SDL_FRect getStartButtonRect(int width, int height) const;
    SDL_FRect getSettingsButtonRect(int width, int height) const;

public:
    void load(SDL_Renderer *renderer, const char *path);
    void render(SDL_Renderer *renderer, int width, int height) const;
    void renderSettings(SDL_Renderer *renderer, int width, int height) const;
    bool isStartButtonPressed(float mouseX, float mouseY, int width, int height) const;
    bool isSettingsButtonPressed(float mouseX, float mouseY, int width, int height) const;
    void cleanup();
};

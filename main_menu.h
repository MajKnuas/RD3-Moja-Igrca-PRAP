#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

class MainMenu {
private:
    SDL_Texture *texture = nullptr;

public:
    bool load(SDL_Renderer *renderer, const char *path);
    void render(SDL_Renderer *renderer, int width, int height) const;
    void cleanup();
};

#include <iostream>
#include <chrono>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h> // Libari za slike ki sem ga posebi rabil installirat in dodat
#include "entity.h"

using namespace std;

struct SDL_State{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *shipTexture;
    SDL_Texture *shipTextureFullHP;
    SDL_Texture *shipTextureHalfHP;
    SDL_Texture *shipTextureLowHP;
    SDL_Texture *sandTexture;
};

SDL_Texture *getShipTexture(float hp, const SDL_State &state) {
    if (hp > 50)
    return state.shipTextureFullHP;

    else if (hp > 20)
    return state.shipTextureHalfHP;

    else 
    return state.shipTextureLowHP;
}

void cleanup(struct SDL_State &state); // protoip funkcije cleanup

//Lastnosti mape 
const int MAP_WIDTH_TILES = 40;
const int MAP_HEIGHT_TILES = 20;
const int TILE_SIZE = 32;



int main(){

    struct SDL_State state;

    int width = 1400;
    int height = 800;
    state.window = SDL_CreateWindow("Main game", width, height, SDL_WINDOW_RESIZABLE);

    // Create rendere
    state.renderer =  SDL_CreateRenderer(state.window, nullptr);

    // Load ship texture
    state.shipTextureFullHP = IMG_LoadTexture(state.renderer, "textures/MeShipFullHP.png");
    state.shipTextureHalfHP = IMG_LoadTexture(state.renderer, "textures/MeShipHalfHP.png");
    state.shipTextureLowHP = IMG_LoadTexture(state.renderer, "textures/MeShipLowHP.png");
    state.sandTexture = IMG_LoadTexture(state.renderer, "textures/Minecraft-Sand-Block.jpg");


    // Create player in center
    Player player(width/2, height/2); // Center for 50x50 square
    
    // Tabela ki preverja če je gumb pritisnjen ali ne + racunanje delta time (cas med 1. framom in 2. framom MOVEMENT OSTANE ENAK HITER NE GLEDENA FPS)
    const bool *keys = SDL_GetKeyboardState(NULL);
    auto lastTime = std::chrono::high_resolution_clock::now(); //auto naredi da nam compiler sam določi podatkovni tip ki ga rabi v tem primeru ni noben katerga smo se mi učili

    // MAIN GAME LOOP
    SDL_Event event;
    bool running = true;
    while(running){ 
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_EVENT_QUIT){
                running=false;
                break; // Ni potrebe vendar za vsak slučaj če bi se še kaj izvajalo da takoj gremo ven iz loopa
            } 
        }
        // Koda da zracuna delta time (cas med 1. framom in 2. framom)
        auto currentTime = std::chrono::high_resolution_clock::now(); //auto naredi da nam compiler sam določi podatkovni tip ki ga rabi v tem primeru ni noben katerga smo se mi učili
        float dt = std::chrono::duration<float>(currentTime - lastTime).count(); // <float> je potreben saj (float) ne bi deloval v tem primeru pravilno saj currentTime - lastTime vrne std::chrono::duration objekt in ne stevilke
        lastTime = currentTime;

        // Movement, preverja gledena zgornjo tabelo ce je gumb drzan
        if(keys[SDL_SCANCODE_W])player.moveUp(dt);
        if(keys[SDL_SCANCODE_S])player.moveDown(dt);
        if(keys[SDL_SCANCODE_A])player.moveLeft(dt);
        if(keys[SDL_SCANCODE_D])player.moveRight(dt);
      

        // risanje renderjev v pomnilnik
        SDL_SetRenderDrawColor(state.renderer, 0, 153, 255, 255);
        SDL_RenderClear(state.renderer); // ciscenje screena

        SDL_FRect rect = {player.getX(), player.getY(), 50, 50}; // {x, y, width, height}
        SDL_FPoint center = { rect.w / 2, rect.h / 2 };  // center of rectangle so it can be rotated around

        // Draw the player (ship texture + rotation while moving)
        SDL_RenderTextureRotated(state.renderer,getShipTexture(player.getHP(), state), NULL, &rect, player.getAngle(), &center, SDL_FLIP_NONE);

        // prikaz vsega ki smo narisali v pomnilnik
        SDL_RenderPresent(state.renderer);


        
    }
    cleanup(state);
    return 0;
}
void createTile(const struct SDLState &state ){

    /*
    1 - PESEK
    2 - NASA LADJA
    3 - 
    */
    int map[MAP_WIDTH_TILES][MAP_HEIGHT_TILES] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
}

// Funkcija da zbriše vse stvari iz pomnilnika da ne pride do overlflowa in nam uniči kodo
void cleanup(struct SDL_State &state){
    SDL_DestroyTexture(state.shipTextureFullHP);
    SDL_DestroyTexture(state.shipTextureHalfHP);
    SDL_DestroyTexture(state.shipTextureLowHP);
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    SDL_Quit();
}

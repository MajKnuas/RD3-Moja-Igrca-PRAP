// NASLEDNJIC LIMIT FPS zato da ni movement prehitr ko zdej se premika glede na fps
#include <iostream>
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


    // Create player in center
    Player player(width/2, height/2); // Center for 50x50 square
    
    // Tabela ki preverja če je gumb pritisnjen ali ne
    const bool *keys = SDL_GetKeyboardState(NULL);

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

        // Movement, preverja gledena zgornjo tabelo ce je gumb drzan
        if(keys[SDL_SCANCODE_W])player.moveUp();
        if(keys[SDL_SCANCODE_S])player.moveDown();
        if(keys[SDL_SCANCODE_A])player.moveLeft();
        if(keys[SDL_SCANCODE_D])player.moveRight();
      

        // risanje renderjev v pomnilnik
        SDL_SetRenderDrawColor(state.renderer, 0, 153, 255, 255);
        SDL_RenderClear(state.renderer); // ciscenje screena

        // Draw the player (ship texture)
        SDL_FRect rect = {player.getX(), player.getY(), 50, 50}; // {x, y, width, height}
        SDL_RenderTexture(state.renderer, getShipTexture(player.getHP(), state), NULL, &rect);

        // prikaz vsega ki smo narisali v pomnilnik
        SDL_RenderPresent(state.renderer);


        
    }
    cleanup(state);
    return 0;
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

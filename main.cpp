#include <iostream>
#include <chrono>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h> // Libari za slike ki sem ga posebi rabil installirat in dodat
#include "entity.h"
#include "main_menu.h"

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



int main(){

    struct SDL_State state;

    // Window size (camera viewport)
    int width = 1920;
    int height = 1080;
    
    state.window = SDL_CreateWindow("Main game", width, height, SDL_WINDOW_RESIZABLE);

    // Create rendere
    state.renderer =  SDL_CreateRenderer(state.window, nullptr);

    // Load ship texture
    state.shipTextureFullHP = IMG_LoadTexture(state.renderer, "textures/MeShipFullHP.png");
    state.shipTextureHalfHP = IMG_LoadTexture(state.renderer, "textures/MeShipHalfHP.png");
    state.shipTextureLowHP = IMG_LoadTexture(state.renderer, "textures/MeShipLowHP.png");
    state.sandTexture = IMG_LoadTexture(state.renderer, "textures/Minecraft-Sand-Block.jpg");

    MainMenu mainMenu;
    mainMenu.load(state.renderer, "textures/MainMenuTexture.png");
    bool inMainMenu = true;


    // Create player in center of map
    Player player(MAP_WIDTH / 2 , MAP_HEIGHT / 2);
    
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

            if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                width = event.window.data1;
                height = event.window.data2;
            }

            if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_F11 && !event.key.repeat){
                // Ko kliknemo f11 da nam da full screen al pa windowed z okvirji 
                Uint32 windowFlags = SDL_GetWindowFlags(state.window);
                if (windowFlags &SDL_WINDOW_MAXIMIZED) {
                    SDL_RestoreWindow(state.window);
                } else {
                    SDL_SetWindowBordered(state.window, true);
                    SDL_MaximizeWindow(state.window);
                }
            }

            if (inMainMenu) {
                if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat && event.key.scancode != SDL_SCANCODE_F11) {
                    inMainMenu = false;
                }

                if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                    inMainMenu = false;
                }
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

        if (inMainMenu) {
            SDL_SetRenderDrawColor(state.renderer, 0, 0, 0, 255);
            SDL_RenderClear(state.renderer);
            mainMenu.render(state.renderer, width, height);
            SDL_RenderPresent(state.renderer);
            continue;
        }

        // Keep player within map boundaries
        player.clampToMap();
        
        // Calculate camera offset to center player on screen
        float cameraX = player.getX() - width / 2.0f;
        float cameraY = player.getY() - height / 2.0f;
        
        // Clamp camera to map boundaries
        if (cameraX < 0)
            cameraX = 0;

        if (cameraX + width > MAP_WIDTH)
            cameraX = MAP_WIDTH - width;

        if (cameraY < 0) 
            cameraY = 0;

        if (cameraY + height > MAP_HEIGHT) 
            cameraY = MAP_HEIGHT - height;
      

        // risanje renderjev v pomnilnik
        SDL_SetRenderDrawColor(state.renderer, 0, 153, 255, 255);
        SDL_RenderClear(state.renderer); // ciscenje screena

        // Risanje peska (obale)
        float rectWidth = MAP_WIDTH / 3;
        float rectX = MAP_WIDTH - rectWidth;
        float rectY = 0;
        
        SDL_FRect pesek = {rectX - cameraX, rectY - cameraY, rectWidth, MAP_HEIGHT};

        // Risanje texutre obale -pesek
        SDL_RenderTexture(state.renderer,state.sandTexture, NULL, &pesek);

        // Draw player relative to camera position
        SDL_FRect rect = {player.getX() - cameraX, player.getY() - cameraY, 50, 50}; // {x, y, width, height}
        SDL_FPoint center = { rect.w / 2, rect.h / 2 };  // center of rectangle so it can be rotated around

        // Draw the player (ship texture + rotation while moving)
        SDL_RenderTextureRotated(state.renderer,getShipTexture(player.getHP(), state), NULL, &rect, player.getAngle(), &center, SDL_FLIP_NONE);

        // prikaz vsega ki smo narisali v pomnilnik
        SDL_RenderPresent(state.renderer);


        
    }
    mainMenu.cleanup();
    cleanup(state);
    return 0;
}


// Funkcija da zbriše vse stvari iz pomnilnika da ne pride do overlflowa in nam uniči kodo
void cleanup(struct SDL_State &state){
    SDL_DestroyTexture(state.shipTextureFullHP);
    SDL_DestroyTexture(state.shipTextureHalfHP);
    SDL_DestroyTexture(state.shipTextureLowHP);
    SDL_DestroyTexture(state.sandTexture);
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    SDL_Quit();
}

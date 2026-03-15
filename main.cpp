// NASLEDNJIC NAREDI DA JE SLIKA LADJE PRIKAZAN NA SCREENU
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h> // Libari za slike ki sem ga posebi rabil installirat in dodat

using namespace std;

struct SDL_State{
    SDL_Window *window;
    SDL_Renderer *renderer;
};

void cleanup(struct SDL_State &state); // protoip funkcije cleanup

int main(){

    struct SDL_State state;

    int width = 1400;
    int height = 800;
    state.window = SDL_CreateWindow("Main game", width, height, SDL_WINDOW_RESIZABLE);

    // Create rendere
    state.renderer =  SDL_CreateRenderer(state.window, nullptr);

    // load game assets
    

    // MAIN GAME LOOP
    SDL_Event event;
    bool running = true;
    while(running){ 
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_EVENT_QUIT){
                running=false;
                break; // Ni potrebe vendar za vsak slučaj če bi se še kaj izvajalo da takoj gremo ven iz loopa
            }
            else if(event.type == SDL_EVENT_KEY_DOWN){
                SDL_Log("Pritisnil si gumb: %d", event.key.key);
            }
           else if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN){
            SDL_Log("Pritisnil si misko na x: %f, y: %f", event.button.x, event.button.y);

           }
        }
        // risanje renderjev v pomnilnik
        SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
        SDL_RenderClear(state.renderer);

        // prikaz vsega ki smo narisali v pomnilnik
        SDL_RenderPresent(state.renderer);


    }
    cleanup(state);
    return 0;
}

// Funkcija da zbriše vse stvari iz pomnilnika da ne pride do overlflowa in nam uniči kodo
void cleanup(struct SDL_State &state){
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    SDL_Quit();
}

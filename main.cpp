#include <iostream>
#include <chrono>
#include <ctime>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <string>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h> // Libari za slike 
#include <SDL3_ttf/SDL_ttf.h>
#include "entity.h"
#include "main_menu.h"

using namespace std;

enum class AppScreen {
    MainMenu,
    LevelSelect,
    Settings,
    Leaderboard,
    UsernameInput,
    Game
};

struct SDL_State{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *shipTexture;
    SDL_Texture *shipTextureFullHP;
    SDL_Texture *shipTextureHalfHP;
    SDL_Texture *shipTextureLowHP;
    SDL_Texture *mePlayer;

    SDL_Texture *sandTexture;
    SDL_Texture *trash;

    SDL_Texture *EnemyshipTexture;
    SDL_Texture *EnemyshipTextureFullHP;
    SDL_Texture *EnemyshipTextureHalfHP;
    SDL_Texture *EnemyshipTextureLowHP;
};

SDL_Texture *getShipTexture(float hp, const Player &player, const SDL_State &state) {
    if (player.isOnSand()) {
        return state.mePlayer;
    }
    
    if (hp > 75)
        return state.shipTextureFullHP;
    else if (hp > 50)
        return state.shipTextureHalfHP;
    else 
        return state.shipTextureLowHP;
}

SDL_Texture *getEnemyShipTexture(float hp, const SDL_State &state) {
    if (hp > 75)
        return state.EnemyshipTextureFullHP;
    else if (hp > 50)
        return state.EnemyshipTextureHalfHP;
    else
        return state.EnemyshipTextureLowHP;
}

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

void cleanup(struct SDL_State &state); // protoip funkcije cleanup

// Rectangle collision detection (AABB - Axis-Aligned Bounding Box)
bool isColliding(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
    return x1 < x2 + w2 &&
           x1 + w1 > x2 &&
           y1 < y2 + h2 &&
           y1 + h1 > y2;
}

int main(){

    struct SDL_State state;

    // Window size (camera viewport)
    int width = 1920;
    int height = 1080;
    
    state.window = SDL_CreateWindow("Main game", width, height, SDL_WINDOW_RESIZABLE);

    // Create rendere
    state.renderer =  SDL_CreateRenderer(state.window, nullptr);

    // Load textures
    state.shipTextureFullHP = IMG_LoadTexture(state.renderer, "textures/MeShipFullHP.png");
    state.shipTextureHalfHP = IMG_LoadTexture(state.renderer, "textures/MeShipHalfHP.png");
    state.shipTextureLowHP = IMG_LoadTexture(state.renderer, "textures/MeShipLowHP.png");

    state.mePlayer =  IMG_LoadTexture(state.renderer, "textures/MePlayer.png");

    state.sandTexture = IMG_LoadTexture(state.renderer, "textures/Minecraft-Sand-Block.jpg");
    state.trash = IMG_LoadTexture(state.renderer, "textures/trash.png");

    state.EnemyshipTextureFullHP = IMG_LoadTexture(state.renderer, "textures/EnemyShipFullHP.png");
    state.EnemyshipTextureHalfHP = IMG_LoadTexture(state.renderer, "textures/EnemyShipHalfHP.png");
    state.EnemyshipTextureLowHP = IMG_LoadTexture(state.renderer, "textures/EnemyShipLowHP.png");

    MainMenu mainMenu;
    mainMenu.load(state.renderer, "textures/MainMenuTexture.png");
    AppScreen currentScreen = AppScreen::MainMenu;
    bool gameOver = false;
    bool victory = false;
    int currentLevel = 0;
    int selectedLevel = 0;
    std::string username;
    int activeEnemyCount = 0;
    float gameTimer = 0.0f;
    TTF_Font *gameFont = TTF_OpenFont("fonts/VCR_OSD_MONO_1.001.ttf", 96);
    TTF_Font *timerFont = TTF_OpenFont("fonts/VCR_OSD_MONO_1.001.ttf", 32);
    SDL_Texture *defeatTexture = nullptr;
    SDL_Texture *victoryTexture = nullptr;
    SDL_Texture *timerTexture = nullptr;
    SDL_Texture *retryTexture = nullptr;
    int defeatWidth = 0;
    int defeatHeight = 0;
    int victoryWidth = 0;
    int victoryHeight = 0;
    int timerWidth = 0;
    int timerHeight = 0;
    int retryWidth = 0;
    int retryHeight = 0;
    if (gameFont) {
        SDL_Color white = {255, 255, 255, 255};
        defeatTexture = createTextTexture(state.renderer, gameFont, "DEFEAT", white, defeatWidth, defeatHeight);
        victoryTexture = createTextTexture(state.renderer, gameFont, "VICTORY", white, victoryWidth, victoryHeight);
        retryTexture = createTextTexture(state.renderer, gameFont, "RETRY", white, retryWidth, retryHeight);
    }

    // Create player in center of map
    Player player(MAP_WIDTH / 2 , MAP_HEIGHT / 2);
    
    // Seed random movement for enemies before any AI target selection
    srand(time(NULL));

    // Max possible enemies (will spawn based on level)
    const int MAX_ENEMY_COUNT = 5;
    Enemy enemies[MAX_ENEMY_COUNT];
    for (int i = 0; i < MAX_ENEMY_COUNT; i++) {
        float x = (i + 1) * (SAND_START_X - PLAYER_SIZE) / (MAX_ENEMY_COUNT + 1);
        float y = (i + 1) * (MAP_HEIGHT - PLAYER_SIZE) / (MAX_ENEMY_COUNT + 1);
        enemies[i] = Enemy(x, y);
    }

    std::vector<SDL_FRect> trashRects;

    auto restartCurrentLevel = [&](int level) {
        currentLevel = level;
        activeEnemyCount = level;
        gameOver = false;
        victory = false;
        gameTimer = 0.0f;
        trashRects.clear();
        player = Player(MAP_WIDTH / 2, MAP_HEIGHT / 2);

        for (int i = 0; i < activeEnemyCount; i++) {
            float x = (i + 1) * (SAND_START_X - PLAYER_SIZE) / (activeEnemyCount + 1);
            float y = (i + 1) * (MAP_HEIGHT - PLAYER_SIZE) / (activeEnemyCount + 1);
            enemies[i] = Enemy(x, y);

            if (currentLevel == 1) {
                enemies[i].setMovementSpeed(140.0f * 2.0f);
                enemies[i].setTrashSpawnInterval(Enemy::TRASH_SPAWN_INTERVAL);
            } else if (currentLevel == 2) {
                enemies[i].setMovementSpeed(140.0f);
                enemies[i].setTrashSpawnInterval(Enemy::TRASH_SPAWN_INTERVAL / 3.0f);
            } else if (currentLevel == 3) {
                enemies[i].setMovementSpeed(140.0f);
                enemies[i].setTrashSpawnInterval(Enemy::TRASH_SPAWN_INTERVAL);
            } else if (currentLevel == 4) {
                enemies[i].setMovementSpeed(140.0f * 25.0f);
                enemies[i].setTrashSpawnInterval(Enemy::TRASH_SPAWN_INTERVAL / 20.0f);
                enemies[i].setDirectionChangeInterval(0.2f);
            } else if (currentLevel == 5) {
                enemies[i].setMovementSpeed(140.0f);
                enemies[i].setTrashSpawnInterval(Enemy::TRASH_SPAWN_INTERVAL);
                enemies[i].setDirectionChangeInterval(0.5f);
            }
        }
    };
    
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

            if (currentScreen == AppScreen::MainMenu && event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
                const float mouseX = event.button.x;
                const float mouseY = event.button.y;

                if (mainMenu.isStartButtonPressed(mouseX, mouseY, width, height)) {
                    currentScreen = AppScreen::LevelSelect;
                } else if (mainMenu.isSettingsButtonPressed(mouseX, mouseY, width, height)) {
                    currentScreen = AppScreen::Settings;
                } else if (mainMenu.isLeaderboardButtonPressed(mouseX, mouseY, width, height)) {
                    currentScreen = AppScreen::Leaderboard;
                }
            }

            if (currentScreen == AppScreen::LevelSelect && event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
                const float mouseX = event.button.x;
                const float mouseY = event.button.y;
                int level = mainMenu.getLevelButtonPressed(mouseX, mouseY, width, height);
                
                if (level > 0) {
                    selectedLevel = level;
                    username.clear();
                    SDL_StartTextInput(state.window);
                    currentScreen = AppScreen::UsernameInput;
                }
            }

            if (currentScreen == AppScreen::UsernameInput) {
                if (event.type == SDL_EVENT_TEXT_INPUT) {
                    username += event.text.text;
                } else if (event.type == SDL_EVENT_KEY_DOWN) {
                    if (event.key.scancode == SDL_SCANCODE_BACKSPACE && !username.empty()) {
                        username.pop_back();
                    } else if (event.key.scancode == SDL_SCANCODE_RETURN && !username.empty()) {
                        SDL_StopTextInput(state.window);
                        restartCurrentLevel(selectedLevel);
                        currentScreen = AppScreen::Game;
                    } else if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
                        SDL_StopTextInput(state.window);
                        currentScreen = AppScreen::LevelSelect;
                    }
                }
            }

            if (currentScreen == AppScreen::Game && gameOver && !victory && event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
                const float mouseX = event.button.x;
                const float mouseY = event.button.y;
                float retryX = (static_cast<float>(width) - static_cast<float>(retryWidth)) * 0.5f;
                float retryY = (static_cast<float>(height) + static_cast<float>(defeatHeight)) * 0.5f + 40.0f;
                SDL_FRect retryRect = { retryX, retryY, static_cast<float>(retryWidth), static_cast<float>(retryHeight) };

                if (mouseX >= retryRect.x && mouseX <= retryRect.x + retryRect.w && mouseY >= retryRect.y && mouseY <= retryRect.y + retryRect.h) {
                    restartCurrentLevel(currentLevel);
                }
            }

            if (currentScreen == AppScreen::LevelSelect && event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat && event.key.scancode == SDL_SCANCODE_ESCAPE) {
                currentScreen = AppScreen::MainMenu;
            }

            if (currentScreen == AppScreen::Settings && event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat && event.key.scancode == SDL_SCANCODE_ESCAPE) {
                currentScreen = AppScreen::MainMenu;
            }

            if (currentScreen == AppScreen::Leaderboard && event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat && event.key.scancode == SDL_SCANCODE_ESCAPE) {
                currentScreen = AppScreen::MainMenu;
            }

            if (currentScreen == AppScreen::UsernameInput && event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat && event.key.scancode == SDL_SCANCODE_ESCAPE) {
                SDL_StopTextInput(state.window);
                currentScreen = AppScreen::LevelSelect;
            }
        }
        // Koda da zracuna delta time (cas med 1. framom in 2. framom)
        auto currentTime = std::chrono::high_resolution_clock::now(); //auto naredi da nam compiler sam določi podatkovni tip ki ga rabi v tem primeru ni noben katerga smo se mi učili
        float dt = std::chrono::duration<float>(currentTime - lastTime).count(); // <float> je potreben saj (float) ne bi deloval v tem primeru pravilno saj currentTime - lastTime vrne std::chrono::duration objekt in ne stevilke
        lastTime = currentTime;

        // Update player slowdown effect
        player.updateSlowdown(dt);

        // Movement, preverja gledena zgornjo tabelo ce je gumb drzan
        if (!gameOver) {
            if(keys[SDL_SCANCODE_W])player.moveUp(dt);
            if(keys[SDL_SCANCODE_S])player.moveDown(dt);
            if(keys[SDL_SCANCODE_A])player.moveLeft(dt);
            if(keys[SDL_SCANCODE_D])player.moveRight(dt);
        }

        if (currentScreen == AppScreen::MainMenu) {
            SDL_RenderClear(state.renderer);
            mainMenu.render(state.renderer, width, height);
            SDL_RenderPresent(state.renderer);
            continue; // Nujno rabi bit ker cene se screeni prekrivajo en cez druzga
        }

        if (currentScreen == AppScreen::LevelSelect) {
            SDL_RenderClear(state.renderer);
            mainMenu.renderLevelSelect(state.renderer, width, height);
            SDL_RenderPresent(state.renderer);
            continue;
        }

        if (currentScreen == AppScreen::Settings) {
            mainMenu.renderSettings(state.renderer, width, height);
            SDL_RenderPresent(state.renderer);
            continue;
        }

        if (currentScreen == AppScreen::Leaderboard) {
            SDL_RenderClear(state.renderer);
            mainMenu.renderLeaderboard(state.renderer, width, height);
            SDL_RenderPresent(state.renderer);
            continue;
        }

        if (currentScreen == AppScreen::UsernameInput) {
            SDL_SetRenderDrawColor(state.renderer, 20, 20, 20, 255);
            SDL_RenderClear(state.renderer);
            
            if (timerFont) {
                SDL_Color white = {255, 255, 255, 255};
                std::string prompt = "Enter username for Level " + std::to_string(selectedLevel) + ":";
                int promptW, promptH;
                SDL_Texture *promptTexture = createTextTexture(state.renderer, timerFont, prompt.c_str(), white, promptW, promptH);
                if (promptTexture) {
                    SDL_FRect promptRect = {(static_cast<float>(width) - promptW) * 0.5f, static_cast<float>(height) * 0.4f, static_cast<float>(promptW), static_cast<float>(promptH)};
                    SDL_RenderTexture(state.renderer, promptTexture, nullptr, &promptRect);
                    SDL_DestroyTexture(promptTexture);
                }
                
                std::string displayText = username;
                if (SDL_GetTicks() % 1000 < 500) {
                    displayText += "|";
                }
                int textW, textH;
                SDL_Texture *textTexture = createTextTexture(state.renderer, timerFont, displayText.c_str(), white, textW, textH);
                if (textTexture) {
                    SDL_FRect textRect = {(static_cast<float>(width) - textW) * 0.5f, static_cast<float>(height) * 0.5f, static_cast<float>(textW), static_cast<float>(textH)};
                    SDL_RenderTexture(state.renderer, textTexture, nullptr, &textRect);
                    SDL_DestroyTexture(textTexture);
                }
                
                std::string instructions = "Press ENTER to start, ESC to go back";
                int instW, instH;
                SDL_Texture *instTexture = createTextTexture(state.renderer, timerFont, instructions.c_str(), white, instW, instH);
                if (instTexture) {
                    SDL_FRect instRect = {(static_cast<float>(width) - instW) * 0.5f, static_cast<float>(height) * 0.6f, static_cast<float>(instW), static_cast<float>(instH)};
                    SDL_RenderTexture(state.renderer, instTexture, nullptr, &instRect);
                    SDL_DestroyTexture(instTexture);
                }
            }
            
            SDL_RenderPresent(state.renderer);
            continue;
        }

        // Keep player within map boundaries
        player.clampToMap();

        // Update game timer
        if (!gameOver && currentScreen == AppScreen::Game) {
            gameTimer += dt;
        }

        // Update enemy movement AI on sea and drop trash only while the game is running
        if (!gameOver) {
            for (int i = 0; i < activeEnemyCount; i++) {
                if (enemies[i].isAlive()) {
                    enemies[i].updateAI(dt);
                    if (enemies[i].updateTrashSpawn(dt)) {
                        trashRects.push_back({enemies[i].getX() + 10.0f, enemies[i].getY() + 10.0f, 100.0f, 100.0f});
                    }
                }
            }

            // Collect trash by touching it; damage is stronger on level 4
            for (int i = static_cast<int>(trashRects.size()) - 1; i >= 0; --i) {
                const SDL_FRect &trashRect = trashRects[i];
                if (isColliding(player.getX(), player.getY(), 50, 50,
                                trashRect.x, trashRect.y, trashRect.w, trashRect.h)) {
                    float damage = (currentLevel == 4) ? 100.0f : 5.0f; // LEVEL 4 INSTAL KILL TRASH
                    player.takeDamage(damage);
                    player.applySlowdown();
                    trashRects.erase(trashRects.begin() + i);
                }
            }
            
            // Check collision between player and enemy
            for (int i = 0; i < activeEnemyCount; i++) {
                if (enemies[i].isAlive() && isColliding(player.getX(), player.getY(), 50, 50, 
                                                enemies[i].getX(), enemies[i].getY(), 50, 50)) {
                    enemies[i].takeDamage(25, dt);
                }
            }

            if (player.getHP() <= 0.0f) {
                gameOver = true;
            }

            // Check victory condition
            if (!victory && !gameOver) {
                if (currentLevel == 4) {
                    // Level 4: survive 60 seconds
                    if (gameTimer >= 60.0f) {
                        victory = true;
                        gameOver = true;
                        mainMenu.saveScore(currentLevel, username, gameTimer);
                    }
                } else {
                    // Other levels: all enemies destroyed AND all trash collected
                    bool allEnemiesDead = true;
                    for (int i = 0; i < activeEnemyCount; i++) {
                        if (enemies[i].isAlive()) {
                            allEnemiesDead = false;
                            break;
                        }
                    }
                    
                    if (allEnemiesDead && trashRects.empty()) {
                        victory = true;
                        gameOver = true;
                        mainMenu.saveScore(currentLevel, username, gameTimer);
                    }
                }
            }
        }
        
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
        float rectWidth = MAP_WIDTH - SAND_START_X;
        float rectX = SAND_START_X;
        float rectY = 0;
        
        SDL_FRect pesek = {rectX - cameraX, rectY - cameraY, rectWidth, MAP_HEIGHT};

        // Risanje texutre obale -pesek
        SDL_RenderTexture(state.renderer,state.sandTexture, NULL, &pesek);

        // Draw player relative to camera position
        SDL_FRect rect = {player.getX() - cameraX, player.getY() - cameraY, 50, 50}; // {x, y, width, height}
        SDL_FPoint center = { rect.w / 2, rect.h / 2 };  // center of rectangle so it can be rotated around

        // Draw the player (ship texture + rotation while moving)
        SDL_RenderTextureRotated(state.renderer,getShipTexture(player.getHP(), player, state), NULL, &rect, player.getAngle(), &center, SDL_FLIP_NONE);

        // Draw enemy ships only if alive
        for (int i = 0; i < activeEnemyCount; i++) {
            if (enemies[i].isAlive()) {
                SDL_FRect enemyRect = {enemies[i].getX() - cameraX, enemies[i].getY() - cameraY, 50, 50};
                SDL_FPoint enemyCenter = { enemyRect.w / 2, enemyRect.h / 2 };
                SDL_RenderTextureRotated(state.renderer, getEnemyShipTexture(enemies[i].getHP(), state), NULL, &enemyRect, enemies[i].getAngle(), &enemyCenter, SDL_FLIP_NONE);
            }
        }

        // Draw trash dropped by enemies
        for (const SDL_FRect &trashRect : trashRects) {
            SDL_FRect worldTrash = { trashRect.x - cameraX, trashRect.y - cameraY, trashRect.w, trashRect.h };
            SDL_RenderTexture(state.renderer, state.trash, NULL, &worldTrash);
        }

        // Draw player HP bar in top-left
        float hpPercent = player.getHP() / 100.0f;
        if (hpPercent < 0.0f) hpPercent = 0.0f;
        if (hpPercent > 1.0f) hpPercent = 1.0f;
        SDL_FRect hpBarBg = {20.0f, 20.0f, 220.0f, 28.0f};
        SDL_FRect hpBarFg = {24.0f, 24.0f, 214.0f * hpPercent, 20.0f};
        SDL_SetRenderDrawColor(state.renderer, 60, 60, 60, 255);
        SDL_RenderFillRect(state.renderer, &hpBarBg);
        SDL_SetRenderDrawColor(state.renderer, 220, 40, 40, 255);
        SDL_RenderFillRect(state.renderer, &hpBarFg);
        SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
        SDL_RenderLine(state.renderer, hpBarBg.x, hpBarBg.y, hpBarBg.x + hpBarBg.w, hpBarBg.y);
        SDL_RenderLine(state.renderer, hpBarBg.x, hpBarBg.y + hpBarBg.h, hpBarBg.x + hpBarBg.w, hpBarBg.y + hpBarBg.h);
        SDL_RenderLine(state.renderer, hpBarBg.x, hpBarBg.y, hpBarBg.x, hpBarBg.y + hpBarBg.h);
        SDL_RenderLine(state.renderer, hpBarBg.x + hpBarBg.w, hpBarBg.y, hpBarBg.x + hpBarBg.w, hpBarBg.y + hpBarBg.h);

        // Draw timer next to HP bar
        if (timerFont) {
            int seconds = static_cast<int>(gameTimer);
            int milliseconds = static_cast<int>((gameTimer - seconds) * 100);
            char timerText[16];
            snprintf(timerText, sizeof(timerText), "Time: %02d.%02d", seconds, milliseconds);
            SDL_Color white = {255, 255, 255, 255};
            if (timerTexture) SDL_DestroyTexture(timerTexture);
            timerTexture = createTextTexture(state.renderer, timerFont, timerText, white, timerWidth, timerHeight);
        }
        if (timerTexture) {
            SDL_FRect timerRect = {250.0f, 18.0f, static_cast<float>(timerWidth), static_cast<float>(timerHeight)};
            SDL_RenderTexture(state.renderer, timerTexture, nullptr, &timerRect);
        }

        if (gameOver && defeatTexture && !victory) {
            SDL_FRect defeatRect = {
                (static_cast<float>(width) - static_cast<float>(defeatWidth)) * 0.5f,
                (static_cast<float>(height) - static_cast<float>(defeatHeight)) * 0.5f - 40.0f,
                static_cast<float>(defeatWidth),
                static_cast<float>(defeatHeight)
            };
            SDL_RenderTexture(state.renderer, defeatTexture, nullptr, &defeatRect);

            if (retryTexture) {
                SDL_FRect retryRect = {
                    (static_cast<float>(width) - static_cast<float>(retryWidth)) * 0.5f,
                    defeatRect.y + defeatRect.h + 40.0f,
                    static_cast<float>(retryWidth),
                    static_cast<float>(retryHeight)
                };
                SDL_SetRenderDrawColor(state.renderer, 30, 30, 30, 200);
                SDL_RenderFillRect(state.renderer, &retryRect);
                SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 255);
                SDL_RenderRect(state.renderer, &retryRect);
                SDL_RenderTexture(state.renderer, retryTexture, nullptr, &retryRect);
            }
        }

        if (victory && victoryTexture) {
            SDL_FRect victoryRect = {
                (static_cast<float>(width) - static_cast<float>(victoryWidth)) * 0.5f,
                (static_cast<float>(height) - static_cast<float>(victoryHeight)) * 0.5f,
                static_cast<float>(victoryWidth),
                static_cast<float>(victoryHeight)
            };
            SDL_RenderTexture(state.renderer, victoryTexture, nullptr, &victoryRect);
        }

        // prikaz vsega ki smo narisali v pomnilnik
        SDL_RenderPresent(state.renderer);


        
    }
    if (defeatTexture) {
        SDL_DestroyTexture(defeatTexture);
        defeatTexture = nullptr;
    }

    if (victoryTexture) {
        SDL_DestroyTexture(victoryTexture);
        victoryTexture = nullptr;
    }

    if (retryTexture) {
        SDL_DestroyTexture(retryTexture);
        retryTexture = nullptr;
    }

    if (timerTexture) {
        SDL_DestroyTexture(timerTexture);
        timerTexture = nullptr;
    }

    if (gameFont) {
        TTF_CloseFont(gameFont);
        gameFont = nullptr;
    }

    if (timerFont) {
        TTF_CloseFont(timerFont);
        timerFont = nullptr;
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
    SDL_DestroyTexture(state.trash);
    SDL_DestroyTexture(state.EnemyshipTextureFullHP);
    SDL_DestroyTexture(state.EnemyshipTextureHalfHP);
    SDL_DestroyTexture(state.EnemyshipTextureLowHP);
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    SDL_Quit();
}

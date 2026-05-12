#include "main_menu.h"
#include <algorithm>
#include <cstdio>
#include <cstring>

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

SDL_FRect MainMenu::getLeaderboardButtonRect(int width, int height) const {
    SDL_FRect settingsRect = getSettingsButtonRect(width, height);
    const float gap = 18.0f;
    return {settingsRect.x, settingsRect.y + settingsRect.h + gap, static_cast<float>(leaderboardLabelWidth), static_cast<float>(leaderboardLabelHeight)};
}

SDL_FRect MainMenu::getLevelButtonRect(int level, int width, int height) const {
    const float buttonWidth = 80.0f;
    const float buttonHeight = 80.0f;
    const float gap = 30.0f;
    const float totalWidth = (buttonWidth * 5) + (gap * 4);
    const float startX = (static_cast<float>(width) - totalWidth) * 0.5f;
    const float startY = static_cast<float>(height) * 0.4f;
    
    const int levelIndex = level - 1;
    return {startX + (levelIndex * (buttonWidth + gap)), startY, buttonWidth, buttonHeight};
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
    leaderboardLabelTexture = createTextTexture(renderer, font, "LEADERBOARD", white, leaderboardLabelWidth, leaderboardLabelHeight);
    
    // Create level button textures
    for (int i = 0; i < 5; i++) {
        char levelText[2] = {static_cast<char>('1' + i), '\0'};
        levelLabelTextures[i] = createTextTexture(renderer, font, levelText, white, levelLabelWidths[i], levelLabelHeights[i]);
    }

    loadLeaderboards();
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
    SDL_FRect leaderboardRect = getLeaderboardButtonRect(width, height);

    if (startLabelTexture) {
        SDL_RenderTexture(renderer, startLabelTexture, nullptr, &startRect);
    }

    if (settingsLabelTexture) {
        SDL_RenderTexture(renderer, settingsLabelTexture, nullptr, &settingsRect);
    }

    if (leaderboardLabelTexture) {
        SDL_RenderTexture(renderer, leaderboardLabelTexture, nullptr, &leaderboardRect);
    }
}

void MainMenu::renderSettings(SDL_Renderer *renderer, int width, int height) const {
    (void)width;
    (void)height;
    SDL_SetRenderDrawColor(renderer, 12, 12, 12, 255);
    SDL_RenderClear(renderer);
}

void MainMenu::renderLevelSelect(SDL_Renderer *renderer, int width, int height) const {
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderClear(renderer);

    // Draw title
    SDL_Color white = {255, 255, 255, 255};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    // Draw 5 level buttons
    for (int i = 0; i < 5; i++) {
        SDL_FRect levelRect = getLevelButtonRect(i + 1, width, height);
        
        // Draw button border
        SDL_RenderRect(renderer, &levelRect);
        
        // Draw level number
        if (levelLabelTextures[i]) {
            SDL_FRect textRect = {
                levelRect.x + (levelRect.w - static_cast<float>(levelLabelWidths[i])) * 0.5f,
                levelRect.y + (levelRect.h - static_cast<float>(levelLabelHeights[i])) * 0.5f,
                static_cast<float>(levelLabelWidths[i]),
                static_cast<float>(levelLabelHeights[i])
            };
            SDL_RenderTexture(renderer, levelLabelTextures[i], nullptr, &textRect);
        }
    }
}

bool MainMenu::isStartButtonPressed(float mouseX, float mouseY, int width, int height) const {
    return pointInRect(mouseX, mouseY, getStartButtonRect(width, height));
}

bool MainMenu::isSettingsButtonPressed(float mouseX, float mouseY, int width, int height) const {
    return pointInRect(mouseX, mouseY, getSettingsButtonRect(width, height));
}

bool MainMenu::isLeaderboardButtonPressed(float mouseX, float mouseY, int width, int height) const {
    return pointInRect(mouseX, mouseY, getLeaderboardButtonRect(width, height));
}

int MainMenu::getLevelButtonPressed(float mouseX, float mouseY, int width, int height) const {
    for (int i = 0; i < 5; i++) {
        if (pointInRect(mouseX, mouseY, getLevelButtonRect(i + 1, width, height))) {
            return i + 1;
        }
    }
    return -1;
}

void MainMenu::loadLeaderboards() {
    for (int level = 1; level <= 5; level++) {
        char filename[32];
        snprintf(filename, sizeof(filename), "leaderboard_level_%d.txt", level);
        FILE *file = fopen(filename, "r");
        if (file) {
            leaderboards[level - 1].clear();
            char line[256];
            while (fgets(line, sizeof(line), file)) {
                char username[128];
                float time;
                if (sscanf(line, "%127[^:]:%f", username, &time) == 2) {
                    leaderboards[level - 1].push_back({username, time});
                }
            }
            fclose(file);
            // Sort by time ascending (lower time is better)
            std::sort(leaderboards[level - 1].begin(), leaderboards[level - 1].end(), 
                      [](const LeaderboardEntry &a, const LeaderboardEntry &b) {
                          return a.time < b.time;
                      });
        }
    }
}

void MainMenu::saveScore(int level, const std::string &username, float time) {
    // Always add the new score
    leaderboards[level - 1].push_back({username, time});
    
    // Sort by time (best times first)
    std::sort(leaderboards[level - 1].begin(), leaderboards[level - 1].end(), 
              [](const LeaderboardEntry &a, const LeaderboardEntry &b) {
                  return a.time < b.time;
              });
    
    // Keep only top 5
    if (leaderboards[level - 1].size() > 5) {
        leaderboards[level - 1].resize(5);
    }
    
    // Save to file
    char filename[32];
    snprintf(filename, sizeof(filename), "leaderboard_level_%d.txt", level);
    FILE *file = fopen(filename, "w");
    if (file) {
        for (const auto &entry : leaderboards[level - 1]) {
            fprintf(file, "%s:%.2f\n", entry.username.c_str(), entry.time);
        }
        fclose(file);
    }
}

void MainMenu::renderLeaderboard(SDL_Renderer *renderer, int width, int height) const {
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderClear(renderer);
    
    if (!font) return;
    
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255};
    
    // Render level leaderboards
    float yOffset = 50.0f;
    for (int level = 1; level <= 5; level++) {
        // Level title
        char title[16];
        snprintf(title, sizeof(title), "Level %d", level);
        int titleW, titleH;
        SDL_Texture *titleTexture = createTextTexture(renderer, font, title, white, titleW, titleH);
        if (titleTexture) {
            SDL_FRect titleRect = {(static_cast<float>(width) - titleW) * 0.5f, yOffset, static_cast<float>(titleW), static_cast<float>(titleH)};
            SDL_RenderTexture(renderer, titleTexture, nullptr, &titleRect);
            SDL_DestroyTexture(titleTexture);
            yOffset += titleH + 10.0f;
        }
        
        // Leaderboard entries
        const auto &board = leaderboards[level - 1];
        for (size_t i = 0; i < board.size() && i < 5; i++) { // Show top 5
            char entryText[256];
            int seconds = static_cast<int>(board[i].time);
            int centiseconds = static_cast<int>((board[i].time - seconds) * 100);
            snprintf(entryText, sizeof(entryText), "%d. %s - %d.%02d", static_cast<int>(i + 1), board[i].username.c_str(), seconds, centiseconds);
            
            int entryW, entryH;
            SDL_Texture *entryTexture = createTextTexture(renderer, font, entryText, i == 0 ? yellow : white, entryW, entryH);
            if (entryTexture) {
                SDL_FRect entryRect = {50.0f, yOffset, static_cast<float>(entryW), static_cast<float>(entryH)};
                SDL_RenderTexture(renderer, entryTexture, nullptr, &entryRect);
                SDL_DestroyTexture(entryTexture);
                yOffset += entryH + 5.0f;
            }
        }
        yOffset += 20.0f; // Space between levels
    }
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

    if (leaderboardLabelTexture) {
        SDL_DestroyTexture(leaderboardLabelTexture);
        leaderboardLabelTexture = nullptr;
    }

    for (int i = 0; i < 5; i++) {
        if (levelLabelTextures[i]) {
            SDL_DestroyTexture(levelLabelTextures[i]);
            levelLabelTextures[i] = nullptr;
        }
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

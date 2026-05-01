#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <vector>
#include <string>

struct LeaderboardEntry {
    std::string username;
    float time;
};

class MainMenu {
private:
    SDL_Texture *texture = nullptr;
    TTF_Font *font = nullptr;
    SDL_Texture *startLabelTexture = nullptr;
    SDL_Texture *settingsLabelTexture = nullptr;
    SDL_Texture *leaderboardLabelTexture = nullptr;
    SDL_Texture *levelLabelTextures[5] = {nullptr};
    int startLabelWidth = 0;
    int startLabelHeight = 0;
    int settingsLabelWidth = 0;
    int settingsLabelHeight = 0;
    int leaderboardLabelWidth = 0;
    int leaderboardLabelHeight = 0;
    int levelLabelWidths[5] = {0};
    int levelLabelHeights[5] = {0};

    std::vector<LeaderboardEntry> leaderboards[5]; // One for each level

    SDL_FRect getStartButtonRect(int width, int height) const;
    SDL_FRect getSettingsButtonRect(int width, int height) const;
    SDL_FRect getLeaderboardButtonRect(int width, int height) const;
    SDL_FRect getLevelButtonRect(int level, int width, int height) const;

public:
    void load(SDL_Renderer *renderer, const char *path);
    void render(SDL_Renderer *renderer, int width, int height) const;
    void renderSettings(SDL_Renderer *renderer, int width, int height) const;
    void renderLevelSelect(SDL_Renderer *renderer, int width, int height) const;
    void renderLeaderboard(SDL_Renderer *renderer, int width, int height) const;
    bool isStartButtonPressed(float mouseX, float mouseY, int width, int height) const;
    bool isSettingsButtonPressed(float mouseX, float mouseY, int width, int height) const;
    bool isLeaderboardButtonPressed(float mouseX, float mouseY, int width, int height) const;
    int getLevelButtonPressed(float mouseX, float mouseY, int width, int height) const;
    void loadLeaderboards();
    void saveScore(int level, const std::string &username, float time);
    void cleanup();
};

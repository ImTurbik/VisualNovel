#pragma once
#include <SDL3/SDL.h>
#include <string>

#include "Player.hpp"

class Game {
public:
    Game() {}
    ~Game() {}
    
    bool init(std::string title, int w, int h);
    void render();
    void update();
    void handleEvents();
    void clean();
    void startGame() { running_ = true; }
    void stopGame() { running_ = false; }
    bool isRunning() { return running_; }

    Player player;

private:
    bool running_ = false;
    SDL_Window* window_;
    SDL_Renderer* renderer_;
};
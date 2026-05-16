#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <vector>

#include "Collider.hpp"
#include "Player.hpp"
#include "Npc.hpp"

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

    // princess greybeard captain cabinBoy
    Npc captain;
    Npc princess;
    Npc greybeard;
    Npc cabinBoy;

    std::vector<Collider> mapColliders;

private:
    bool running_ = false;
    SDL_Window* window_;
    SDL_Renderer* renderer_;
};
#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <vector>

#include "Collider.hpp"
#include "Player.hpp"
#include "Npc.hpp"
#include "DialogueSystem.hpp"

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

    void drawText(const std::string& text, float x, float y, SDL_Color color);

private:
    bool running_ = false;
    bool inDialogue_ = false;
    std::string currentPortrait_ = "";
    
    int dialogueStep_ = 0;
    int currentDialogueNodeId_ = 0;
    std::vector<DialogueNode> dialogueDatabase_;
    void buildDialogueDatabase();
    TTF_Font* font_ = nullptr;

    SDL_Window* window_;
    SDL_Renderer* renderer_;
};
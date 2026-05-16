#include "Game.hpp"
#include <iostream>

#include "TextureManager.hpp"
#include "InputHandler.hpp"
#include "Player.hpp"

bool Game::init(std::string title, int w, int h) {
    if (SDL_Init(SDL_INIT_VIDEO)) { 
        window_ = SDL_CreateWindow(title.c_str(), w, h, 0);
        if (window_ != 0) {
            std::cout << "window created" << std::endl;
            renderer_ = SDL_CreateRenderer(window_, NULL);
            if (renderer_ != 0) {
                std::cout << "renderer created" << std::endl;
            } else {
                std::cerr << "renderer error" << std::endl;
                return false;
            }
        } else {
            std::cerr << "window error" << std::endl;
            return false;
        }
    }


    TextureManager::Instance().load("images/map/map_ship.png", "deck", renderer_);
    
    TextureManager::Instance().load("images/player/player_idle.png", "player_idle", renderer_);
    TextureManager::Instance().load("images/player/player_step1.png", "player_step1", renderer_);
    TextureManager::Instance().load("images/player/player_step2.png", "player_step2", renderer_);

    // princess greybeard captain cabinBoy
    TextureManager::Instance().load("images/npc/captain.png", "captain_texture", renderer_);
    TextureManager::Instance().load("images/npc/princess.png", "princess_texture", renderer_);
    TextureManager::Instance().load("images/npc/greybeard.png", "greybeard_texture", renderer_);
    TextureManager::Instance().load("images/npc/cabinBoy.png", "cabinBoy_texture", renderer_);

    player.load("player_idle", 400, 400, 128, 128);

    captain.load("captain_texture", 800, 320, 128, 128);
    princess.load("princess_texture", 400, 400, 128, 128);
    greybeard.load("greybeard_texture", 800, 100, 128, 128);
    cabinBoy.load("cabinBoy_texture", 320, 100, 128, 128);
    
    startGame();
    return true;
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            stopGame();
        }

        InputHandler::Instance()->handle(event);
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);

    TextureManager::Instance().draw("deck", 0, 0, 1280, 720, renderer_);
    // TextureManager::Instance().draw("player", 400, 400, 128, 128, renderer_);
    
    captain.draw(renderer_);
    princess.draw(renderer_);
    greybeard.draw(renderer_);
    cabinBoy.draw(renderer_);

    player.draw(renderer_);

    SDL_RenderPresent(renderer_);
}

void Game::update() {
    player.update();
    captain.update();
    princess.update();
    greybeard.update();
    cabinBoy.update();
}

void Game::clean() {
    std::cout << "Cleaning game..." << std::endl;
    if (renderer_ != nullptr) {
        SDL_DestroyRenderer(renderer_);
    }
    if (window_ != nullptr) {
        SDL_DestroyWindow(window_);
    }
    SDL_Quit();
}

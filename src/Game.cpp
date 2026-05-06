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
    TextureManager::Instance().load("images/player/player_idle.png", "player", renderer_);

    player.load("player", 400, 400, 128, 128);

    
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
    player.draw(renderer_);

    SDL_RenderPresent(renderer_);
}

void Game::update() {
    player.update();
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

#define SDL_MAIN_HANDLED
#include "Game.hpp"

int main(int argc, char* argv[]) {
    Game* game = new Game();

    if (game->init("Visual Novel", 1280, 720)) {
        while (game->isRunning()) {
            game->handleEvents();
            game->update();
            game->render();
        }
    }

    game->clean();
    delete game;
    
    return 0;
}

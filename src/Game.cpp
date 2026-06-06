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

    player.load("player_idle", 430, 320, 128, 128);

    captain.load("captain_texture", 900, 400, 128, 128);
    princess.load("princess_texture", 550, 80, 128, 128);
    greybeard.load("greybeard_texture", 830, 90, 128, 128);
    cabinBoy.load("cabinBoy_texture", 320, 100, 128, 128);
    

    mapColliders.clear();

    // границы корабля
    mapColliders.push_back(Collider(0, 0, 1280, 120));     // верхний борт
    mapColliders.push_back(Collider(0, 0, 350, 720));      // левый борт + мачта
    mapColliders.push_back(Collider(940, 0, 340, 720));    // правый борт + штурвал   

    // нижний борт + трап
    mapColliders.push_back(Collider(0, 540, 480, 180));
    mapColliders.push_back(Collider(630, 560, 720, 160));

    // крупные объекты на палубе
    mapColliders.push_back(Collider(250, 420, 180, 130));  // якорь под мачтой
    mapColliders.push_back(Collider(655, 420, 180, 140));  // бочки

    // загрузка портретов для системы диалогов
    TextureManager::Instance().load("images/portraits/captain_neutral.png", "captain_portrait", renderer_);
    TextureManager::Instance().load("images/portraits/princess_neutral.png", "princess_portrait", renderer_);
    TextureManager::Instance().load("images/portraits/greybeard_neutral.png", "greybeard_portrait", renderer_);
    TextureManager::Instance().load("images/portraits/cabinBoy_neutral.png", "cabinBoy_portrait", renderer_);

    inDialogue_ = false;
    currentPortrait_ = "";


    startGame();
    return true;
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            stopGame();
        }

        // проверяем клик левой кнопки мыши
        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
            if (inDialogue_) {
                // если диалог уже открыт - закрываем его любым следующим кликом
                inDialogue_ = false;
                currentPortrait_ = "";
            } else {
                // если мы просто ходим - проверяем, попал ли клик по NPC
                float mx = event.button.x;
                float my = event.button.y;

                auto cap = captain.getBounds();
                auto prin = princess.getBounds();
                auto grey = greybeard.getBounds();
                auto boy = cabinBoy.getBounds();

                // условия попадания курсора мыши в рамки хитбоксов персонажей
                if (mx >= cap.x && mx <= cap.x + cap.w && my >= cap.y && my <= cap.y + cap.h) {
                    inDialogue_ = true; currentPortrait_ = "captain_portrait";
                }
                else if (mx >= prin.x && mx <= prin.x + prin.w && my >= prin.y && my <= prin.y + prin.h) {
                    inDialogue_ = true; currentPortrait_ = "princess_portrait";
                }
                else if (mx >= grey.x && mx <= grey.x + grey.w && my >= grey.y && my <= grey.y + grey.h) {
                    inDialogue_ = true; currentPortrait_ = "greybeard_portrait";
                }
                else if (mx >= boy.x && mx <= boy.x + boy.w && my >= boy.y && my <= boy.y + boy.h) {
                    inDialogue_ = true; currentPortrait_ = "cabinBoy_portrait";
                }
            }
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

    // ОТРИСОВКА КОЛЛАЙДЕРОВ
    // for (auto& wall : mapColliders) {
    //     wall.drawDebug(renderer_);
    // }


    // если идёт диалог - рисуем интерфейс поверх всего
    if (inDialogue_ && currentPortrait_ != "") {
        SDL_FRect blackScreen = {0, 0, 1280, 720};
        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 140);
        SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(renderer_, &blackScreen);

        TextureManager::Instance().draw(currentPortrait_, 830, 120, 450, 600, renderer_);

        SDL_FRect box = {50, 580, 1180, 100};
        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 200);
        SDL_RenderFillRect(renderer_, &box);

        SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
        SDL_RenderRect(renderer_, &box);
    }


    SDL_RenderPresent(renderer_);
}

void Game::update() {
    // если открыт диалог - полностью выходим из метода, замораживая физику игрока
    if (inDialogue_) {
        return;
    }

    // обновляем всех NPC
    captain.update();
    princess.update();
    greybeard.update();
    cabinBoy.update();


    // КОЛЛИЗИИ

    // запоминаем исходные координаты
    SDL_FRect originalPos = player.getBounds();

    player.update();

    float targetX = player.getBounds().x;
    float targetY = player.getBounds().y;
    std::string animatedTag = player.getTextureTag();

    // проверка по X
    player.load(animatedTag, targetX, originalPos.y, originalPos.w, originalPos.h);
    for (const auto& wall : mapColliders) {
        if (Collider::checkCollision(player.getBounds(), wall.bounds)) {
            targetX = originalPos.x; // если врезался боком - отменяем X
            break;
        }
    }

    // проверка по Y
    player.load(animatedTag, targetX, targetY, originalPos.w, originalPos.h);
    for (const auto& wall : mapColliders) {
        if (Collider::checkCollision(player.getBounds(), wall.bounds)) {
            targetY = originalPos.y; // если врезался ногами/головой - отменяем Y
            break;
        }
    }

    // окончательное применение координат
    player.load(animatedTag, targetX, targetY, originalPos.w, originalPos.h);
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

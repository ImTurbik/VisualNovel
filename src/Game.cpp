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

    if (!TTF_Init()) {
        std::cerr << "TTF Init Error" << std::endl;
        return false;
    }

    // загружаем шрифт
    font_ = TTF_OpenFont("fonts/arial.ttf", 20);
    if (font_ == nullptr) {
        std::cerr << "Font Load Error" << std::endl;
        return false;
    }

    inDialogue_ = false;
    currentPortrait_ = "";
    dialogueStep_ = 0;

    buildDialogueDatabase();

    startGame();
    return true;
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            stopGame();
        }

        // если диалог идет - ловим нажатия цифр (1, 2, 3) для выбора веток
        if (inDialogue_ && event.type == SDL_EVENT_KEY_DOWN) {
            DialogueNode* current = nullptr;
            for (auto& node : dialogueDatabase_) {
                if (node.id == currentDialogueNodeId_) { current = &node; break; }
            }

            if (current != nullptr && !current->choices.empty()) {
                if (event.key.scancode == SDL_SCANCODE_1) currentDialogueNodeId_ = current->choices[0].nextNodeId;
                else if (event.key.scancode == SDL_SCANCODE_2) currentDialogueNodeId_ = current->choices[1].nextNodeId;
                else if (event.key.scancode == SDL_SCANCODE_3) currentDialogueNodeId_ = current->choices[2].nextNodeId;
            }
        }

        // ловим клик мышки
        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
            if (inDialogue_) {
                DialogueNode* current = nullptr;
                for (auto& node : dialogueDatabase_) {
                    if (node.id == currentDialogueNodeId_) { current = &node; break; }
                }

                if (current != nullptr && current->choices.empty()) {
                    // если развилок нет, обычный клик двигает сюжет к следующему узлу
                    currentDialogueNodeId_ = current->nextNodeId;

                    if (currentDialogueNodeId_ == -1) {
                        inDialogue_ = false;
                        currentPortrait_ = "";
                    }
                }
            } else {
                // открытие диалога при клике на Юнгу
                float mx = event.button.x; float my = event.button.y;
                auto boy = cabinBoy.getBounds();

                if (mx >= boy.x && mx <= boy.x + boy.w && my >= boy.y && my <= boy.y + boy.h) {
                    inDialogue_ = true;
                    currentPortrait_ = "cabinBoy_portrait";
                    currentDialogueNodeId_ = 0;
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

        SDL_FRect box = {50, 580, 1180, 130};
        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 200);
        SDL_RenderFillRect(renderer_, &box);

        SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
        SDL_RenderRect(renderer_, &box);

        DialogueNode* current = nullptr;
        for (auto& node : dialogueDatabase_) {
            if (node.id == currentDialogueNodeId_) { 
                current = &node; 
                break; 
            }
        }

        if (current != nullptr) {
            SDL_Color white = { 255, 255, 255, 255 };
            SDL_Color yellow = { 255, 255, 0, 255 };

            drawText(current->npcName + ":", 70, 585, yellow);

            if (!current->choices.empty()) {
                float currentY = 610;
                for (const auto& choice : current->choices) {
                    drawText(choice.text, 70, currentY, white);
                    currentY += 24;
                }
            } else {
                drawText(current->text, 70, 615, white);
            }
        }
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

void Game::buildDialogueDatabase() {
    dialogueDatabase_.clear();

    // узел 0: стартовый вопрос Марко и развилка
    DialogueNode node0;
    node0.id = 0;
    node0.npcName = "Марко";
    node0.text = "Здрасьте, сир. Так вы и в самом деле не человек?";
    node0.choices.push_back(DialogueChoice{"1. Отшутиться", 1});
    node0.choices.push_back(DialogueChoice{"2. Приструнить", 2});
    node0.choices.push_back(DialogueChoice{"3. Ответить спокойно", 3});
    node0.nextNodeId = -1;
    dialogueDatabase_.push_back(node0);

    // узел 1: ответ "Отшутиться"
    DialogueNode node1;
    node1.id = 1;
    node1.npcName = "Имотэс";
    node1.text = "«Меня выдали уши?» — ты улыбнулся. Марко рассмеялся в ответ.";
    node1.nextNodeId = 4; // переход на финальную реплику
    dialogueDatabase_.push_back(node1);

    // узел 2: ответ "Приструнить"
    DialogueNode node2;
    node2.id = 2;
    node2.npcName = "Имотэс";
    node2.text = "«Следи за языком перед власть имущими» — холодно отрезал ты.";
    node2.nextNodeId = 4;
    dialogueDatabase_.push_back(node2);

    // узел 3: ответ "Спокойно ответить"
    DialogueNode node3;
    node3.id = 3;
    node3.npcName = "Имотэс";
    node3.text = "«Человек. Разве что не обычный» — буднично ответил ты.";
    node3.nextNodeId = 4;
    dialogueDatabase_.push_back(node3);

    // узел 4: финал сцены знакомства
    DialogueNode node4;
    node4.id = 4;
    node4.npcName = "Марко";
    node4.text = "Меня Марко звать. Юнгой тут служу. Чего обсудить хотели, сир?";
    node4.nextNodeId = -1; // конец диалога
    dialogueDatabase_.push_back(node4);
}

void Game::drawText(const std::string& text, float x, float y, SDL_Color color) {
    if (font_ == nullptr || text.empty()) return;

    SDL_Surface* surface = TTF_RenderText_Blended(font_, text.c_str(), 0, color);
    if (surface != nullptr) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
        if (texture != nullptr) {
            SDL_FRect dstRect = { x, y, (float)surface->w, (float)surface->h };
            SDL_RenderTexture(renderer_, texture, NULL, &dstRect);
            SDL_DestroyTexture(texture);
        }
        SDL_DestroySurface(surface);
    }
}
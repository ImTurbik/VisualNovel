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

        // если диалог идет - ловим нажатия цифр для выбора веток
        if (inDialogue_ && event.type == SDL_EVENT_KEY_DOWN) {
            DialogueNode* current = nullptr;
            for (auto& node : dialogueDatabase_) {
                if (node.id == currentDialogueNodeId_) { 
                    current = &node; 
                    break; 
                }
            }

            if (current != nullptr && !current->choices.empty()) {
                int choiceIndex = -1;
                if (event.key.scancode == SDL_SCANCODE_1) choiceIndex = 0;
                else if (event.key.scancode == SDL_SCANCODE_2) choiceIndex = 1;
                else if (event.key.scancode == SDL_SCANCODE_3) choiceIndex = 2;
                else if (event.key.scancode == SDL_SCANCODE_4) choiceIndex = 3;

                if (choiceIndex >= 0 && choiceIndex < (int)current->choices.size()) {
                    currentDialogueNodeId_ = current->choices[choiceIndex].nextNodeId;
                    
                    // Если nextNodeId == -1 — выходим из диалога
                    if (currentDialogueNodeId_ == -1) {
                        inDialogue_ = false;
                        currentPortrait_ = "";
                        std::cout << "Dialogue ended by choice" << std::endl;
                    }
                }
            }
        }

        // ловим клик мышки
        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
            if (inDialogue_) {
                DialogueNode* current = nullptr;
                for (auto& node : dialogueDatabase_) {
                    if (node.id == currentDialogueNodeId_) { 
                        current = &node; 
                        break; 
                    }
                }

                if (current != nullptr) {
                    if (current->choices.empty()) {
                        // обычный клик — следующий узел
                        currentDialogueNodeId_ = current->nextNodeId;
                        if (currentDialogueNodeId_ == -1) {
                            inDialogue_ = false;
                            currentPortrait_ = "";
                        }
                    }
                    // если есть выборы — ничего не делаем (ждём нажатия 1,2,3)
                }
            } 
            else {
                // === КЛИК ПО NPC НА ПАЛУБЕ ===
                float mx = event.button.x; 
                float my = event.button.y;
                
                // Юнга Марко (cabinBoy)
                auto boy = cabinBoy.getBounds();
                if (mx >= boy.x && mx <= boy.x + boy.w && my >= boy.y && my <= boy.y + boy.h) {
                    inDialogue_ = true;
                    currentPortrait_ = "cabinBoy_portrait";
                    currentDialogueNodeId_ = 0;
                    std::cout << "Dialogue with Marco started" << std::endl;
                    return;
                }

                // Капитан
                auto cap = captain.getBounds();
                if (mx >= cap.x && mx <= cap.x + cap.w && my >= cap.y && my <= cap.y + cap.h) {
                    inDialogue_ = true;
                    currentPortrait_ = "captain_portrait";
                    currentDialogueNodeId_ = 10;
                    std::cout << "Dialogue with Captain started" << std::endl;
                    return;
                }

                // Принцесса
                auto prin = princess.getBounds();
                if (mx >= prin.x && mx <= prin.x + prin.w && my >= prin.y && my <= prin.y + prin.h) {
                    inDialogue_ = true;
                    currentPortrait_ = "princess_portrait";
                    currentDialogueNodeId_ = 20;
                    std::cout << "Dialogue with Princess started" << std::endl;
                    return;
                }

                // Барнабас Грей (greybeard)
                auto beard = greybeard.getBounds();
                if (mx >= beard.x && mx <= beard.x + beard.w && my >= beard.y && my <= beard.y + beard.h) {
                    inDialogue_ = true;
                    currentPortrait_ = "greybeard_portrait";
                    currentDialogueNodeId_ = 30;
                    std::cout << "Dialogue with Greybeard started" << std::endl;
                    return;
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

    // ДИАЛОГ С МАРКО: ID 0-9
    DialogueNode node0;
    node0.id = 0;
    node0.npcName = "Марко";
    node0.text = "Здрасьте, достопочтенный сир. — Так вы, м-м…и в самом деле не человек?";
    node0.choices.clear();
    node0.nextNodeId = 5;
    dialogueDatabase_.push_back(node0);

    DialogueNode node5;
    node5.id = 5;
    node5.npcName = "Марко";
    node5.text = "";  
    node5.choices.push_back(DialogueChoice{"1. Отшутиться", 1});
    node5.choices.push_back(DialogueChoice{"2. Приструнить", 2});
    node5.choices.push_back(DialogueChoice{"3. Спокойно ответить", 3});
    node5.nextNodeId = -1;
    dialogueDatabase_.push_back(node5);

    // Реакции игрока
    DialogueNode node1; node1.id = 1; node1.npcName = "Имотэс"; 
    node1.text = "«Меня выдали уши?» — произносишь ты, не в силах сдержать смешок удивления."; 
    node1.nextNodeId = 4; dialogueDatabase_.push_back(node1);

    DialogueNode node2; node2.id = 2; node2.npcName = "Имотэс"; 
    node2.text = "«Следи за языком в присутствии власть имущих» — холодно отрезаешь ты."; 
    node2.nextNodeId = 4; dialogueDatabase_.push_back(node2);

    DialogueNode node3; node3.id = 3; node3.npcName = "Имотэс"; 
    node3.text = "«Человек, человек. Разве что не обычный.» — буднично отвечаешь ты пожав плечами."; 
    node3.nextNodeId = 4; dialogueDatabase_.push_back(node3);

    // Финальная реплика Марко
    DialogueNode node4;
    node4.id = 4;
    node4.npcName = "Марко";
    node4.text = "Меня Марко звать. Юнгой тут пока являюсь… пока. — он заговорщически подмигнул тебе.";
    node4.nextNodeId = -1;
    dialogueDatabase_.push_back(node4);

    // ДИАЛОГ С КАПИТАНОМ: ID 10-19
    DialogueNode node10;
    node10.id = 10;
    node10.npcName = "Себастьян";
    node10.text = "А, наш дорогой гость! Решили подышать свежим морским воздухом, или всё же снизошли до общения с простым людом?";
    node10.choices.push_back(DialogueChoice{"1. Расскажете о себе?", 11});
    node10.choices.push_back(DialogueChoice{"2. Что мне следует знать о вашем экипаже?", 12});
    node10.choices.push_back(DialogueChoice{"3. Что вы знаете о нашей миссии?", 13});
    node10.choices.push_back(DialogueChoice{"4. Уйти", -1});
    dialogueDatabase_.push_back(node10);

    DialogueNode node11; node11.id = 11; node11.npcName = "Себастьян"; 
    node11.text = "Зовут Джон Сильвер. Ха-ха!.. Нет, конечно же нет. Я — капитан Вандервуд. Для вас могу побыть и просто Себастьяном."; 
    node11.nextNodeId = 10; dialogueDatabase_.push_back(node11);

    DialogueNode node12; node12.id = 12; node12.npcName = "Себастьян"; 
    node12.text = "Экипаж мал: юнга Марко, Морская Принцесса, старый Барнабас Грей. Будьте аккуратнее."; 
    node12.nextNodeId = 10; dialogueDatabase_.push_back(node12);

    DialogueNode node13; node13.id = 13; node13.npcName = "Себастьян"; 
    node13.text = "От меня зависит, чтобы вы добрались до Скрытых Земель целым. Подробностей я лишён."; 
    node13.nextNodeId = 10; dialogueDatabase_.push_back(node13);

    // ДИАЛОГ С ПРИНЦЕССОЙ: ID 20-29
    DialogueNode node20;
    node20.id = 20;
    node20.npcName = "Принцесса";
    node20.text = "Ну и что понадобилось столь августейшей особе в моей скромной компании?";
    node20.choices.push_back(DialogueChoice{"1. Не знал, что в плавание берут женщин.", 21});
    node20.choices.push_back(DialogueChoice{"2. Если ты член экипажа, вероятно, обладаешь навыками?", 22});
    node20.choices.push_back(DialogueChoice{"3. Просто осматриваюсь.", 23});
    node20.choices.push_back(DialogueChoice{"4. Уйти", -1});
    dialogueDatabase_.push_back(node20);

    DialogueNode node21; node21.id = 21; node21.npcName = "Принцесса"; 
    node21.text = "Ну конечно, женщины на корабле — к беде? В такие стереотипы верят только сухопутные."; 
    node21.nextNodeId = -1; dialogueDatabase_.push_back(node21);

    DialogueNode node22; node22.id = 22; node22.npcName = "Принцесса"; 
    node22.text = "В море важны только навыки. Раз вас отправили — у вас их тоже хватает."; 
    node22.nextNodeId = -1; dialogueDatabase_.push_back(node22);

    DialogueNode node23; node23.id = 23; node23.npcName = "Принцесса"; 
    node23.text = "На борту есть более интересный собеседник — Барнабас со своими байками."; 
    node23.nextNodeId = -1; dialogueDatabase_.push_back(node23);

    // ДИАЛОГ С БАРНАБАСОМ ГРЕЕМ: ID 30-39
    DialogueNode node30;
    node30.id = 30;
    node30.npcName = "Барнабас";
    node30.text = "Вы, значится, нашенская важнецкая персона?";
    node30.choices.push_back(DialogueChoice{"1. Кажется, у вас больше всего опыта.", 31});
    node30.choices.push_back(DialogueChoice{"2. Как зовут вашу птицу?", 32});
    node30.choices.push_back(DialogueChoice{"3. Расскажите интересную историю.", 33});
    node30.choices.push_back(DialogueChoice{"4. Уйти", -1});
    dialogueDatabase_.push_back(node30);

    DialogueNode node31; node31.id = 31; node31.npcName = "Барнабас"; 
    node31.text = "Когда-то я запрыгнул зайцем на корабль... Сейчас мне уже за семьдесят!"; 
    node31.nextNodeId = 30; dialogueDatabase_.push_back(node31);

    DialogueNode node32; node32.id = 32; node32.npcName = "Барнабас"; 
    node32.text = "Птицу зовут Крекер. История долгая — в порту Альба-Секка..."; 
    node32.nextNodeId = 30; dialogueDatabase_.push_back(node32);

    DialogueNode node33; node33.id = 33; node33.npcName = "Барнабас"; 
    node33.text = "Поверье про утопленников с монетой... Клянусь своим глазом!"; 
    node33.nextNodeId = 30; dialogueDatabase_.push_back(node33);

    std::cout << "Dialogue database built with " << dialogueDatabase_.size() << " nodes." << std::endl;
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
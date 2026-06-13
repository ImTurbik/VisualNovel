/**
 * @file Game.cpp
 * @brief Реализация основных методов управления игровым процессом класса Game.
 */

#include "Game.hpp"
#include <iostream>

#include "TextureManager.hpp"
#include "InputHandler.hpp"
#include "Player.hpp"

/**
 * @brief Инициализирует подсистемы игры, загружает ресурсы и настраивает начальное состояние.
 * 
 * Метод выполняет пошаговую инициализацию SDL2, создание окна и рендерера,
 * загрузку текстур карты, персонажей, портретов, а также инициализацию
 * шрифтов SDL_ttf. Дополнительно создаются жестко заданные коллайдеры корабля.
 * 
 * @param title Заголовок создаваемого игрового окна.
 * @param w Ширина окна в пикселях.
 * @param h Высота окна в пикселях.
 * @return true Если вся инициализация прошла успешно.
 * @return false Если произошла ошибка на любом из этапов (создание окна, рендерера, загрузка шрифтов).
 */
bool Game::init(std::string title, int w, int h) {
    // Инициализация видео-подсистемы SDL
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

    // Загрузка текстуры палубы корабля (карты)
    TextureManager::Instance().load("images/map/map_ship.png", "deck", renderer_);
    
    // Загрузка анимаций игрока
    TextureManager::Instance().load("images/player/player_idle.png", "player_idle", renderer_);
    TextureManager::Instance().load("images/player/player_step1.png", "player_step1", renderer_);
    TextureManager::Instance().load("images/player/player_step2.png", "player_step2", renderer_);

    // Загрузка спрайтов NPC (Капитан, Принцесса, Седобородый, Юнга)
    TextureManager::Instance().load("images/npc/captain.png", "captain_texture", renderer_);
    TextureManager::Instance().load("images/npc/princess.png", "princess_texture", renderer_);
    TextureManager::Instance().load("images/npc/greybeard.png", "greybeard_texture", renderer_);
    TextureManager::Instance().load("images/npc/cabinBoy.png", "cabinBoy_texture", renderer_);

    // Инициализация стартовой позиции и размеров игрока
    player.load("player_idle", 430, 320, 128, 128);

    // Расстановка NPC на палубе
    captain.load("captain_texture", 900, 400, 128, 128);
    princess.load("princess_texture", 550, 80, 128, 128);
    greybeard.load("greybeard_texture", 830, 90, 128, 128);
    cabinBoy.load("cabinBoy_texture", 320, 100, 128, 128);
    
    // Очистка старых и генерация твердых границ (коллизий) корабля
    mapColliders.clear();

    // Границы корабля (стены и борта)
    mapColliders.push_back(Collider(0, 0, 1280, 120));     ///< Верхний борт
    mapColliders.push_back(Collider(0, 0, 350, 720));      ///< Левый борт + мачта
    mapColliders.push_back(Collider(940, 0, 340, 720));    ///< Правый борт + штурвал   

    // Нижний борт + проход на трап
    mapColliders.push_back(Collider(0, 540, 480, 180));
    mapColliders.push_back(Collider(630, 560, 720, 160));

    // Крупные статичные объекты на палубе
    mapColliders.push_back(Collider(250, 420, 180, 130));  ///< Якорь под мачтой
    mapColliders.push_back(Collider(655, 420, 180, 140));  ///< Бочки

    // Загрузка 2D-портретов персонажей для интерфейса диалогов
    TextureManager::Instance().load("images/portraits/captain_neutral.png", "captain_portrait", renderer_);
    TextureManager::Instance().load("images/portraits/princess_neutral.png", "princess_portrait", renderer_);
    TextureManager::Instance().load("images/portraits/greybeard_neutral.png", "greybeard_portrait", renderer_);
    TextureManager::Instance().load("images/portraits/cabinBoy_neutral.png", "cabinBoy_portrait", renderer_);

    // Инициализация библиотеки работы со шрифтами SDL_ttf
    if (!TTF_Init()) {
        std::cerr << "TTF Init Error" << std::endl;
        return false;
    }

    // Загрузка основного шрифта для текста
    font_ = TTF_OpenFont("fonts/arial.ttf", 20);
    if (font_ == nullptr) {
        std::cerr << "Font Load Error" << std::endl;
        return false;
    }

    // Сброс флагов и переменных состояния диалога
    inDialogue_ = false;
    currentPortrait_ = "";
    dialogueStep_ = 0;

    // Включение вступительной заставки (интро)
    showIntro_ = true;
    introStep_ = 0;

    // Формирование графа диалогов и запуск игрового цикла
    buildDialogueDatabase();
    startGame();
    
    return true;
}

/**
 * @brief Обрабатывает входящие события SDL (ввод пользователя, мышь, клавиатура).
 * 
 * Метод опрашивает очередь событий и распределяет логику в зависимости от текущего состояния:
 * - **В режиме диалога:** Перехватывает клавиши 1-4 для выбора вариантов ответа или клики мыши для прокрутки текста.
 * - **В режиме интро:** Перехватывает Space (Пробел) для переключения страниц заставки.
 * - **В обычном режиме:** Обрабатывает клики левой кнопкой мыши по хитбоксам NPC для начала беседы.
 */
void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // Проверка закрытия окна игры
        if (event.type == SDL_EVENT_QUIT) {
            stopGame();
        }

        // Логика выбора вариантов ответа в диалоге через горячие клавиши (1-4)
        if (inDialogue_ && event.type == SDL_EVENT_KEY_DOWN) {
            DialogueNode* current = nullptr;
            for (auto& node : dialogueDatabase_) {
                if (node.id == currentDialogueNodeId_) { 
                    current = &node; 
                    break; 
                }
            }

            // Проверка нажатия цифровых клавиш, если у текущего узла диалога есть развилки
            if (current != nullptr && !current->choices.empty()) {
                int choiceIndex = -1;
                if (event.key.scancode == SDL_SCANCODE_1) choiceIndex = 0;
                else if (event.key.scancode == SDL_SCANCODE_2) choiceIndex = 1;
                else if (event.key.scancode == SDL_SCANCODE_3) choiceIndex = 2;
                else if (event.key.scancode == SDL_SCANCODE_4) choiceIndex = 3;

                // Переход на следующий узел по индексу выбора
                if (choiceIndex >= 0 && choiceIndex < (int)current->choices.size()) {
                    currentDialogueNodeId_ = current->choices[choiceIndex].nextNodeId;
                    
                    // Если индекс следующего узла равен -1 — диалог завершается
                    if (currentDialogueNodeId_ == -1) {
                        inDialogue_ = false;
                        currentPortrait_ = "";
                        std::cout << "Dialogue ended by choice" << std::endl;
                    }
                }
            }
        }

        // Логика прокрутки страниц интро/вступления по нажатию клавиши Space
        if (showIntro_ && !inDialogue_ && event.type == SDL_EVENT_KEY_DOWN) {
            if (event.key.scancode == SDL_SCANCODE_SPACE) {
                introStep_++;
                if (introStep_ >= 4) {
                    showIntro_ = false; ///< Отключаем интро после 4 шагов
                }
            }
        }

        // Обработка кликов мыши
        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
            if (inDialogue_) {
                // Прокрутка обычного диалога (без развилок) по клику мыши
                DialogueNode* current = nullptr;
                for (auto& node : dialogueDatabase_) {
                    if (node.id == currentDialogueNodeId_) { 
                        current = &node; 
                        break; 
                    }
                }

                if (current != nullptr) {
                    if (current->choices.empty()) {
                        // Обычный клик переводит на следующий ID. Если там -1 — закрываем окно диалога
                        currentDialogueNodeId_ = current->nextNodeId;
                        if (currentDialogueNodeId_ == -1) {
                            inDialogue_ = false;
                            currentPortrait_ = "";
                        }
                    }
                    // Если у узла есть выбор веток (choices), обычный клик игнорируется
                }
            } 
            else {
                // === КЛИК ПО NPC НА ПАЛУБЕ ===
                float mx = event.button.x; 
                float my = event.button.y;
                
                // Проверка клика по Юнге Марко (cabinBoy) -> узел диалога 0
                auto boy = cabinBoy.getBounds();
                if (mx >= boy.x && mx <= boy.x + boy.w && my >= boy.y && my <= boy.y + boy.h) {
                    inDialogue_ = true;
                    currentPortrait_ = "cabinBoy_portrait";
                    currentDialogueNodeId_ = 0;
                    std::cout << "Dialogue with Marco started" << std::endl;
                    return;
                }

                // Проверка клика по Капитану -> узел диалога 10
                auto cap = captain.getBounds();
                if (mx >= cap.x && mx <= cap.x + cap.w && my >= cap.y && my <= cap.y + cap.h) {
                    inDialogue_ = true;
                    currentPortrait = "captain_portrait";
                    currentDialogueNodeId_ = 10;
                    std::cout << "Dialogue with Captain started" << std::endl;
                    return;
                }

                // Проверка клика по Принцессе -> узел диалога 20
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

        // Передача события в глобальный обработчик ввода
        InputHandler::Instance()->handle(event);
    }
}

/**
 * @brief Главный метод рендеринга игры.
 * 
 * Отвечает за отрисовку всего игрового состояния. Если активен режим интро,
 * выводит текстовые экраны сюжета. В противном случае отрисовывает текстуру палубы,
 * всех игровых персонажей (NPC) и самого игрока.
 */
void Game::render() {

    // Очистка экрана черным цветом
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);

    // === РЕЖИМ ОТОБРАЖЕНИЯ ИНТРО-ЗАСТАВКИ ===
    if (showIntro_) {
        SDL_FRect blackScreen = {0, 0, 1280, 720};
        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 240);
        SDL_RenderFillRect(renderer_, &blackScreen);

        SDL_Color white = {255, 255, 255, 255};
        SDL_Color yellow = {255, 255, 0, 255};

        // Пошаговый вывод текстовых слайдов сюжета
        if (introStep_ == 0) {
            drawText("Каюта", 150, 150, yellow);
            drawText("Яркий свет... Резкий толчок почти сваливает тебя с кровати.", 100, 220, white);
            drawText("Ты в каюте корабля. Голова болит. Пора выходить на палубу.", 100, 270, white);
            drawText("Нажми SPACE чтобы продолжить...", 100, 400, white);
        } 
        else if (introStep_ == 1) {
            drawText("Ты привёл себя в порядок, принял лекарство и оделся.", 100, 220, white);
            drawText("В дверь постучали: «Эй, мистер посол, вас на палубе заждались!»", 100, 270, white);
        } 
        else if (introStep_ == 2) {
            drawText("Ты поднимаешься на палубу.", 100, 220, white);
            drawText("Яркий дневной свет бьёт в глаза. Солёный ветер хлещет по лицу.", 100, 270, white);
        } 
        else if (introStep_ == 3) {
            drawText("На палубе тебя ждёт команда:", 100, 200, yellow);
            drawText("• Марко (юнга)", 150, 260, white);
            drawText("• Капитан Себастьян", 150, 290, white);
            drawText("• Принцесса", 150, 320, white);
            drawText("• Барнабас Грей", 150, 350, white);
            drawText("Кликай на них мышкой. Нажми SPACE чтобы начать игру...", 100, 420, white);
        }

        // Вывод буфера интро-экрана на дисплей
        SDL_RenderPresent(renderer_);
        return;
    }

    // === ОСНОВНОЙ ИГРОВОЙ РЕНДЕР ===
    // Отрисовка фонового изображения палубы (карты)
    TextureManager::Instance().draw("deck", 0, 0, 1280, 720, renderer_);
    
    // Отрисовка неигровых персонажей (NPC)
    captain.draw(renderer_);
    princess.draw(renderer_);
    greybeard.draw(renderer_);
    cabinBoy.draw(renderer_);

    // Отрисовка главного героя поверх фона и NPC
    player.draw(renderer_);

    // ОТРИСОВКА КОЛЛАЙДЕРОВ
    // for (auto& wall : mapColliders) {
    //     wall.drawDebug(renderer_);
    // }


    // === ОТРЕСОВКА ИНТЕРФЕЙСА ДИАЛОГА ===
    // Если запущен диалог и задан портрет собеседника, рисуем UI поверх игрового мира
    if (inDialogue_ && currentPortrait_ != "") {
        // Затемнение заднего плана (полупрозрачный черный фон на весь экран)
        SDL_FRect blackScreen = {0, 0, 1280, 720};
        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 140);
        SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(renderer_, &blackScreen);

        // Отрисовка портрета текущего NPC справа
        TextureManager::Instance().draw(currentPortrait_, 830, 120, 450, 600, renderer_);

        // Отрисовка подложки для текстового диалогового окна (внизу экрана)
        SDL_FRect box = {50, 580, 1180, 130};
        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 200);
        SDL_RenderFillRect(renderer_, &box);

        // Отрисовка белой рамки вокруг диалогового окна
        SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
        SDL_RenderRect(renderer_, &box);

        // Поиск текущей ноды диалога в базе данных по её ID
        DialogueNode* current = nullptr;
        for (auto& node : dialogueDatabase_) {
            if (node.id == currentDialogueNodeId_) { 
                current = &node; 
                break; 
            }
        }

        // Если нода найдена, выводим имя NPC и текст/варианты ответа
        if (current != nullptr) {
            SDL_Color white = { 255, 255, 255, 255 };
            SDL_Color yellow = { 255, 255, 0, 255 };

            // Выводим имя говорящего персонажа желтым цветом
            drawText(current->npcName + ":", 70, 585, yellow);

            if (!current->choices.empty()) {
                // Если есть развилка, выводим список доступных вариантов ответов игрока
                float currentY = 610;
                for (const auto& choice : current->choices) {
                    drawText(choice.text, 70, currentY, white);
                    currentY += 24; ///< Смещение по вертикали для каждой следующей строки
                }
            } else {
                // Если вариантов ответа нет, выводим обычную реплику NPC
                drawText(current->text, 70, 615, white);
            }
        }
    }

    // Вывод сформированного кадра из бэк-буфера на экран
    SDL_RenderPresent(renderer_);
}

/**
 * @brief Обновление игровой логики и физики за один кадр.
 * 
 * Метод обрабатывает состояния игры (интро, диалог), обновляет анимации и 
 * координаты NPC, а также передвигает игрока с использованием поосной проверки 
 * столкновений (AABB) со статичными объектами окружения.
 */
void Game::update() {
    // В режиме заставки физика и логика игрового мира не обновляются
    if (showIntro_) {
        return;
    }

    // Во время открытого диалога замораживаем перемещение и физику объектов
    if (inDialogue_) {
        return;
    }

    // Пошаговое обновление состояния и анимаций всех NPC на палубе
    captain.update();
    princess.update();
    greybeard.update();
    cabinBoy.update();


    // === ОБРАБОТКА КОЛЛИЗИЙ ИГРОКА ===

    // Запоминаем позицию игрока до перемещения в этом кадре
    SDL_FRect originalPos = player.getBounds();

    // Запрашиваем новые (желаемые) координаты игрока на основе ввода
    player.update();

    float targetX = player.getBounds().x;
    float targetY = player.getBounds().y;
    std::string animatedTag = player.getTextureTag();

    // Поосная проверка столкновений (Раздельный сдвиг по X и Y предотвращает "прилипание" к стенам)
    
    // ЭТАП 1: Проверка столкновения по оси X
    player.load(animatedTag, targetX, originalPos.y, originalPos.w, originalPos.h);
    for (const auto& wall : mapColliders) {
        if (Collider::checkCollision(player.getBounds(), wall.bounds)) {
            targetX = originalPos.x; // Отмена движения по горизонтали при столкновении
            break;
        }
    }

    // ЭТАП 2: Проверка столкновения по оси Y (уже с учетом скорректированного X)
    player.load(animatedTag, targetX, targetY, originalPos.w, originalPos.h);
    for (const auto& wall : mapColliders) {
        if (Collider::checkCollision(player.getBounds(), wall.bounds)) {
            targetY = originalPos.y; // Отмена движения по вертикали при столкновении
            break;
        }
    }

    // Финальное применение безопасных (проверенных на коллизии) координат к игроку
    player.load(animatedTag, targetX, targetY, originalPos.w, originalPos.h);
}

/**
 * @brief Освобождение ресурсов и корректное завершение работы подсистем.
 * 
 * Уничтожает контекст рендерера SDL, главное окно приложения и деинициализирует 
 * саму библиотеку SDL для предотвращения утечек памяти.
 */
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

/**
 * @brief Инициализирует и строит базу данных игровых диалогов.
 * 
 * Метод полностью очищает текущую базу данных диалогов и заново формирует 
 * ориентированный граф реплик для каждого NPC. Каждая нода (DialogueNode) 
 * представляет собой отдельный экран диалога с текстом, именем говорящего, 
 * списком интерактивных выборов или указателем на следующий логический шаг.
 * 
 * Разделение ID по персонажам:
 * - ID 0-9: Диалоговая ветка юнги Марко.
 * - ID 10-19: Диалоговая ветка капитана Себастьяна.
 */
void Game::buildDialogueDatabase() {
    // Очистка старых данных перед заполнением
    dialogueDatabase_.clear();

    // ==========================================
    // ДИАЛОГ С МАРКО: ID 0-9
    // ==========================================
    
    /** 
     * @brief Вступление Марко.
     * Автоматически перенаправляет игрока на узел выбора (ID 5).
     */
    DialogueNode node0;
    node0.id = 0;
    node0.npcName = "Марко";
    node0.text = "Здрасьте, достопочтенный сир. — Так вы, м-м…и в самом деле не человек?";
    node0.choices.clear();
    node0.nextNodeId = 5; ///< Автоматический переход к развилке ответов
    dialogueDatabase_.push_back(node0);

    /** 
     * @brief Развилка ответов для игрока.
     * Предоставляет 3 варианта реплик, ведущих на разные реактивные узлы.
     */
    DialogueNode node5;
    node5.id = 5;
    node5.npcName = "Марко";
    node5.text = "";  ///< Пустой текст, так как этот узел служит контейнером для меню выбора
    node5.choices.push_back(DialogueChoice{"1. Отшутиться", 1});
    node5.choices.push_back(DialogueChoice{"2. Приструнить", 2});
    node5.choices.push_back(DialogueChoice{"3. Спокойно ответить", 3});
    node5.nextNodeId = -1; ///< Ожидает явного выбора от пользователя (клик/клавиша)
    dialogueDatabase_.push_back(node5);

    // --- Реакции главного героя (Имотэса) ---

    // Вариант 1: Игрок отшучивается. Ведет к финалу (ID 4)
    DialogueNode node1; node1.id = 1; node1.npcName = "Имотэс"; 
    node1.text = "«Меня выдали уши?» — произносишь ты, не в силах сдержать смешок удивления."; 
    node1.nextNodeId = 4; dialogueDatabase_.push_back(node1);

    // Вариант 2: Игрок проявляет строгость. Ведет к финалу (ID 4)
    DialogueNode node2; node2.id = 2; node2.npcName = "Имотэс"; 
    node2.text = "«Следи за языком в присутствии власть имущих» — холодно отрезаешь ты."; 
    node2.nextNodeId = 4; dialogueDatabase_.push_back(node2);

    // Вариант 3: Игрок отвечает нейтрально. Ведет к финалу (ID 4)
    DialogueNode node3; node3.id = 3; node3.npcName = "Имотэс"; 
    node3.text = "«Человек, человек. Разве что не обычный.» — буднично отвечаешь ты пожав плечами."; 
    node3.nextNodeId = 4; dialogueDatabase_.push_back(node3);

    /** 
     * @brief Завершающая реплика Марко.
     * Закрывает текущую диалоговую ветку.
     */
    DialogueNode node4;
    node4.id = 4;
    node4.npcName = "Марко";
    node4.text = "Меня Марко звать. Юнгой тут пока являюсь… пока. — он заговорщически подмигнул тебе.";
    node4.nextNodeId = -1; ///< Конец диалога
    dialogueDatabase_.push_back(node4);

    // ==========================================
    // ДИАЛОГ С КАПИТАНОМ: ID 10-19
    // ==========================================
    
    /** 
     * @brief Главное меню диалога с капитаном Себастьяном.
     * Разветвляет диалог на вопросы о нем, экипаже, миссии или позволяет выйти.
     */
    DialogueNode node10;
    node10.id = 10;
    node10.npcName = "Себастьян";
    node10.text = "А, наш дорогой гость! Решили подышать свежим морским воздухом, или всё же снизошли до общения с простым людом?";
    node10.choices.push_back(DialogueChoice{"1. Расскажете о себе?", 11});
    node10.choices.push_back(DialogueChoice{"2. Что мне следует знать о вашем экипаже?", 12});
    node10.choices.push_back(DialogueChoice{"3. Что вы знаете о нашей миссии?", 13});
    node10.choices.push_back(DialogueChoice{"4. Уйти", -1}); ///< Вариант немедленного выхода из диалога
    dialogueDatabase_.push_back(node10);

     // --- Подветки ответов Капитана (Возвращают игрока в главное меню Капитана, ID 10) ---

    // Вариант 1: Вопрос о личности капитана. Редирект на ID 10.
    DialogueNode node11; node11.id = 11; node11.npcName = "Себастьян"; 
    node11.text = "Зовут Джон Сильвер. Ха-ха!.. Нет, конечно же нет. Я — капитан Вандервуд. Для вас могу побыть и просто Себастьян."; 
    node11.nextNodeId = 10; dialogueDatabase_.push_back(node11);

    // Вариант 2: Вопрос о составе экипажа судна. Редирект на ID 10.
    DialogueNode node12; node12.id = 12; node12.npcName = "Себастьян"; 
    node12.text = "Экипаж мал: юнга Марко, Морская Принцесса, старый Барнабас Грей. Будьте аккуратнее."; 
    node12.nextNodeId = 10; dialogueDatabase_.push_back(node12);

    // Вариант 3: Вопрос о целях текущей экспедиции. Редирект на ID 10.
    DialogueNode node13; node13.id = 13; node13.npcName = "Себастьян"; 
    node13.text = "От меня зависит, чтобы вы добрались до Скрытых Земель целым. Подробностей я лишён."; 
    node13.nextNodeId = 10; dialogueDatabase_.push_back(node13);

    // ==========================================
    // ДИАЛОГ С ПРИНЦЕССОЙ: ID 20-29
    // ==========================================
    
    /** 
     * @brief Главное меню диалога с Морской Принцессой.
     * Позволяет узнать о роли женщин на корабле, её навыках, 
     * получить совет или покинуть беседу.
     */
    DialogueNode node20;
    node20.id = 20;
    node20.npcName = "Принцесса";
    node20.text = "Ну и что понадобилось столь августейшей особе в моей скромной компании?";
    node20.choices.push_back(DialogueChoice{"1. Не знал, что в плавание берут женщин.", 21});
    node20.choices.push_back(DialogueChoice{"2. Если ты член экипажа, вероятно, обладаешь навыками?", 22});
    node20.choices.push_back(DialogueChoice{"3. Просто осматриваюсь.", 23});
    node20.choices.push_back(DialogueChoice{"4. Уйти", -1}); ///< Выход из диалога
    dialogueDatabase_.push_back(node20);

    // --- Подветки ответов Принцессы (Все возвращают в её главное меню, ID 20) ---

    // Вариант 1: Реакция на стереотип о женщинах на корабле. Редирект на ID 20.
    DialogueNode node21; node21.id = 21; node21.npcName = "Принцесса"; 
    node21.text = "Ну конечно, женщины на корабле — к беде? В такие стереотипы верят только сухопутные."; 
    node21.nextNodeId = 20; dialogueDatabase_.push_back(node21);

    // Вариант 2: Ответ о профессионализме и равенстве в море. Редирект на ID 20.
    DialogueNode node22; node22.id = 22; node22.npcName = "Принцесса"; 
    node22.text = "В море важны только навыки. Раз вас отправили — у вас их тоже хватает."; 
    node22.nextNodeId = 20; dialogueDatabase_.push_back(node22);

    // Вариант 3: Нейтральный ответ с советом поговорить с Барнабасом. Редирект на ID 20.
    DialogueNode node23; node23.id = 23; node23.npcName = "Принцесса"; 
    node23.text = "На борту есть более интересный собеседник — Барнабас со своими байками."; 
    node23.nextNodeId = 20; dialogueDatabase_.push_back(node23);

    // ==========================================
    // ДИАЛОГ С БАРНАБАСОМ ГРЕЕМ: ID 30-39
    // ==========================================
    
    /** 
     * @brief Главное меню диалога со старым пиратом Барнабасом Греем.
     * Открывает доступ к историям о его прошлом, его ручном попугае Крекере 
     * и морских суевериях экипажа.
     */
    DialogueNode node30;
    node30.id = 30;
    node30.npcName = "Барнабас";
    node30.text = "Вы, значится, нашенская важнецкая персона?";
    node30.choices.push_back(DialogueChoice{"1. Кажется, у вас больше всего опыта.", 31});
    node30.choices.push_back(DialogueChoice{"2. Как зовут вашу птицу?", 32});
    node30.choices.push_back(DialogueChoice{"3. Расскажите интересную историю.", 33});
    node30.choices.push_back(DialogueChoice{"4. Уйти", -1}); ///< Выход из диалога
    dialogueDatabase_.push_back(node30);

    // --- Подветки ответов Барнабаса (Все возвращают в его главное меню, ID 30) ---

    // Вариант 1: Рассказ о начале морской карьеры и возрасте. Редирект на ID 30.
    DialogueNode node31; node31.id = 31; node31.npcName = "Барнабас"; 
    node31.text = "Когда-то я запрыгнул зайцем на корабль... Сейчас мне уже за семьдесят!"; 
    node31.nextNodeId = 30; dialogueDatabase_.push_back(node31);

    // Вариант 2: Рассказ о питомце и портовом городе Альба-Секка. Редирект на ID 30.
    DialogueNode node32; node32.id = 32; node32.npcName = "Барнабас"; 
    node32.text = "Птицу зовут Крекер. История долгая — в порту Альба-Секка..."; 
    node32.nextNodeId = 30; dialogueDatabase_.push_back(node32);

    // Вариант 3: Мрачная байка про морские поверья и монеты мертвецов. Редирект на ID 30.
    DialogueNode node33; node33.id = 33; node33.npcName = "Барнабас"; 
    node33.text = "Поверье про утопленников с монетой... Клянусь своим глазом!"; 
    node33.nextNodeId = 30; dialogueDatabase_.push_back(node33);

    // Логирование успешного завершения генерации графа диалогов
    std::cout << "Dialogue database built with " << dialogueDatabase_.size() << " nodes." << std::endl;
}

/**
 * @brief Отрисовывает текстовую строку на экране с использованием TrueType шрифта.
 * 
 * Метод динамически создает текстуру из переданной строки текста с помощью SDL_ttf,
 * выводит её в заданных координатах на экран и сразу же освобождает выделенную память.
 * Для сглаживания шрифта применяется качественный режим рендеринга Blended.
 * 
 * @param text Строка текста, которую необходимо отобразить.
 * @param x Координата по оси X (левый верхний угол начала строки).
 * @param y Координата по оси Y (левый верхний угол начала строки).
 * @param color Цвет текста в формате SDL_Color (RGBA).
 */
void Game::drawText(const std::string& text, float x, float y, SDL_Color color) {
    // Проверка на инициализацию шрифта и наличие текста для рендеринга
    if (font_ == nullptr || text.empty()) return;

    // Шаг 1: Создание временной SDL_Surface с качественным (сглаженным) текстом в ОЗУ
    SDL_Surface* surface = TTF_RenderText_Blended(font_, text.c_str(), 0, color);
    if (surface != nullptr) {
        
        // Шаг 2: Перенос пиксельных данных из ОЗУ в видеопамять (создание текстуры для GPU)
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
        if (texture != nullptr) {
            
            // Определение целевого прямоугольника на экране на основе исходного размера текста
            SDL_FRect dstRect = { x, y, (float)surface->w, (float)surface->h };
            
            // Отрисовка текстуры текста в заданные координаты рендерера
            SDL_RenderTexture(renderer_, texture, NULL, &dstRect);
            
            // Освобождение памяти GPU, выделенной под текстуру текущего кадра
            SDL_DestroyTexture(texture);
        }
        
        // Освобождение оперативной памяти, выделенной под временную поверхность
        SDL_DestroySurface(surface);
    }
}
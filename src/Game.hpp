#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <vector>

#include "Collider.hpp"
#include "Player.hpp"
#include "Npc.hpp"
#include "DialogueSystem.hpp"

/**
 * @brief Основной класс игрового движка, управляющий жизненным циклом игры.
 * @details Координирует инициализацию, игровой цикл (ввод, обновление, рендеринг),
 *          систему диалогов, коллизии и управление игровыми сущностями.
 */
class Game {
public:
    /**
     * @brief Конструктор по умолчанию объекта Game.
     */
    Game() {}
	
	    /**
     * @brief Деструктор объекта Game.
     */
    ~Game() {}
    
	 /**
     * @brief Инициализирует подсистемы SDL, создает окно и рендерер.
     * 
     * @param title Заголовок окна игры.
     * @param w Ширина окна в пикселях.
     * @param h Высота окна в пикселях.
     * @return true Если инициализация прошла успешно.
     * @return false Если произошла ошибка при инициализации.
     */
    bool init(std::string title, int w, int h);
	
	 /**
     * @brief Отрисовывает текущий игровой кадр.
     */
    void render();
	
	  /**
     * @brief Обновляет игровую логику (положения объектов, таймеры, состояния).
     */
    void update();
	
	 /**
     * @brief Обрабатывает ввод пользователя и системные события SDL.
     */
    void handleEvents();
	
	 /**
     * @brief Освобождает все выделенные ресурсы (шрифты, рендерер, окно) при выходе.
     */
    void clean();
	
	 /**
     * @brief Запускает главный игровой цикл, переводя флаг running_ в true.
     */
    void startGame() { running_ = true; }
	
	    /**
     * @brief Останавливает главный игровой цикл, переводя флаг running_ в false.
     */
    void stopGame() { running_ = false; }
	
	    /**
     * @brief Проверяет, запущен ли в данный момент игровой цикл.
     * 
     * @return true Если игра выполняется.
     * @return false Если игра должна завершить работу.
     */
    bool isRunning() { return running_; }

 /** @brief Объект игрока, управляемый пользователем. */
    Player player;

	 /** @brief NPC: Капитан корабля. */
    Npc captain;
	
	 /** @brief NPC: Принцесса. */
    Npc princess;
	
	/** @brief NPC: Седобородый (Greybeard). */
    Npc greybeard;
	
	    /** @brief NPC: Юнга (Cabin Boy). */
    Npc cabinBoy;

 /** @brief Список статических коллайдеров игровой карты для проверки столкновений. */
    std::vector<Collider> mapColliders;

/**
     * @brief Отрисовывает текстовую строку на экране.
     * 
     * @param text Выводимый текст.
     * @param x Координата X начала текста.
     * @param y Координата Y начала текста.
     * @param color Цвет текста в формате SDL_Color.
     */
    void drawText(const std::string& text, float x, float y, SDL_Color color);

private:
	/** @brief Флаг работы главного цикла игры. */
    bool running_ = false;
	
	/** @brief Флаг, указывающий, находится ли игрок в режиме диалога. */
    bool inDialogue_ = false;
	
	/** @brief Идентификатор или путь к текущему отображаемому портрету собеседника. */
    std::string currentPortrait_ = "";

	/** @brief Флаг отображения вступительной заставки/интро. */
    bool showIntro_ = true;
	/** @brief Текущий шаг или фаза воспроизведения интро. */
    int introStep_ = 0;
    
	/** @brief Текущий шаг внутри активной реплики диалога. */
    int dialogueStep_ = 0;
	
	/** @brief Идентификатор текущего активного узла диалога из базы данных. */
    int currentDialogueNodeId_ = 0;
	
	** @brief База данных, содержащая все узлы и реплики диалогов в игре. */
    std::vector<DialogueNode> dialogueDatabase_;
	
	/**
     * @brief Заполняет базу данных диалогов (dialogueDatabase_) предустановленными репликами.
     */
    void buildDialogueDatabase();
	
	 /** @brief Указатель на шрифт SDL_ttf, используемый для отрисовки текста. */
    TTF_Font* font_ = nullptr;

	/** @brief Указатель на главное окно SDL. */
    SDL_Window* window_;
	
	/** @brief Указатель на рендерер SDL для отрисовки графики. */
    SDL_Renderer* renderer_;
};
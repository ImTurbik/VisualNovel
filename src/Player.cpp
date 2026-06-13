/**
 * @file Player.cpp
 * @brief Реализация класса главного героя (Игрока).
 */

#include "Player.hpp"
#include "InputHandler.hpp"

/**
 * @brief Загружает и инициализирует параметры игрока.
 * 
 * Передает базовые пространственные координаты, габариты и стартовый 
 * текстурный тег в родительский метод GameObject::load.
 * 
 * @param tag Уникальный строковый идентификатор текстуры игрока в TextureManager.
 * @param x Начальная координата игрока по оси X.
 * @param y Начальная координата игрока по оси Y.
 * @param w Ширина спрайта игрока на экране.
 * @param h Высота спрайта игрока на экране.
 */
void Player::load(std::string tag, float x, float y, float w, float h) {
    GameObject::load(tag, x, y, w, h);
}

/**
 * @brief Отрисовывает текущий спрайт игрока на экране.
 * 
 * Вызывает базовый метод отрисовки кадра из класса GameObject, используя 
 * актуальные координаты и текущий установленный текстурный тег (анимацию).
 * 
 * @param renderer Указатель на активный контекст рендеринга SDL.
 */
void Player::draw(SDL_Renderer* renderer) { GameObject::draw(renderer); }

/**
 * @brief Обновляет логику перемещения и анимацию игрока за один кадр.
 * 
 * Метод опрашивает состояния клавиш движения (W, A, S, D) через InputHandler 
 * и изменяет координаты персонажа. Если игрок движется, динамически переключает 
 * текстурные теги (кадры анимации бега) на основе системного времени SDL.
 */
void Player::update() {
    float speed = 0.08f;     ///< Скорость перемещения игрока (пикселей за итерацию)
    bool isMoving = false;   ///< Флаг, фиксирующий факт движения игрока в текущем кадре

    // --- Обработка ввода (Управление WASD) ---
    if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_W)) { y_ -= speed; isMoving = true; };
    if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_S)) { y_ += speed; isMoving = true; };
    if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_A)) { x_ -= speed; isMoving = true; };
    if (InputHandler::Instance()->isKeyDown(SDL_SCANCODE_D)) { x_ += speed; isMoving = true; };

    // --- Управление покадровой анимацией ---
    if (isMoving) {
        // Расчет текущего кадра на основе прошедшего времени (каждые 150 миллисекунд — новый кадр)
        // Оператор % 3 ограничивает цикл анимации тремя доступными состояниями
        int frame = (SDL_GetTicks() / 150) % 3;

        // Динамическая подмена текстурного тега в зависимости от фазы шага
        if (frame == 0) texture_tag_ = "player_idle";
        else if (frame == 1) texture_tag_ = "player_step1";
        else if (frame == 2) texture_tag_ = "player_step2";
    } else {
        // Если клавиши не зажаты — принудительно возвращаем спрайт покоя
        texture_tag_ = "player_idle";
    }
}

/**
 * @brief Освобождает ресурсы, связанные с объектом игрока.
 * 
 * Вызывает базовую очистку памяти из GameObject::clean().
 */
void Player::clean() { GameObject::clean(); }
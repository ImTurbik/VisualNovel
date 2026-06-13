/**
 * @file InputHandler.cpp
 * @brief Реализация менеджера ввода данных (клавиатура и мышь).
 */

#include "InputHandler.hpp"

// Инициализация статического указателя на единственный экземпляр класса
InputHandler* InputHandler::instance_ = 0;

/**
 * @brief Конструктор класса по умолчанию.
 * 
 * Инициализирует массив состояний кнопок мыши (5 стандартных кнопок) значениями false 
 * и выделяет память под вектор для хранения координат курсора.
 */
InputHandler::InputHandler() {
    for (int i = 0; i < 5; i++) {
        mouseStates_.push_back(false);
    }
    mousePosition_ = new Vector2D(0, 0);
}

/**
 * @brief Деструктор класса.
 * @note Если Vector2D не является умным указателем, здесь может потребоваться delete mousePosition_;
 */
InputHandler::~InputHandler() {}

/**
 * @brief Возвращает текущее состояние указанной кнопки мыши.
 * 
 * @param buttonNumber Индекс кнопки мыши (обычно используются перечисления LEFT, MIDDLE, RIGHT и т.д.).
 * @return true Если кнопка зажата.
 * @return false Если кнопка отпущена.
 */
bool InputHandler::getMouseButtonState(int buttonNumber) {
    return mouseStates_[buttonNumber];
}

/**
 * @brief Обработчик события нажатия кнопки мыши.
 * 
 * Проверяет, какая именно кнопка была нажата на основе системного события SDL, 
 * обновляет её состояние в массиве mouseStates_ и выводит отладочное сообщение.
 * 
 * @param event Структура системного события SDL_Event.
 */
void InputHandler::onMouseButtonDown(SDL_Event event) {
    if (event.button.button == SDL_BUTTON_LEFT) {
        mouseStates_[LEFT] = true;
        std::cout << "left" << std::endl;
    }
    if (event.button.button == SDL_BUTTON_MIDDLE) {
        mouseStates_[MIDDLE] = true;
        std::cout << "mid" << std::endl;
    }
    if (event.button.button == SDL_BUTTON_RIGHT) {
        mouseStates_[RIGHT] = true;
        std::cout << "right" << std::endl;
    }
    if (event.button.button == SDL_BUTTON_X1) {
        mouseStates_[BACK] = true;
        std::cout << "back" << std::endl;
    }
    if (event.button.button == SDL_BUTTON_X2) {
        mouseStates_[FORW] = true;
        std::cout << "forw" << std::endl;
    }
}

/**
 * @brief Обработчик события отпускания кнопки мыши.
 * 
 * Сбрасывает состояние соответствующей кнопки в массиве mouseStates_ в значение false.
 * 
 * @param event Структура системного события SDL_Event.
 */
void InputHandler::onMouseButtonUp(SDL_Event event) {
    if (event.button.button == SDL_BUTTON_LEFT) {
        mouseStates_[LEFT] = false;
    }
    if (event.button.button == SDL_BUTTON_MIDDLE) {
        mouseStates_[MIDDLE] = false;
    }
    if (event.button.button == SDL_BUTTON_RIGHT) {
        mouseStates_[RIGHT] = false;
    }
    if (event.button.button == SDL_BUTTON_X1) {
        mouseStates_[BACK] = false;
    }
    if (event.button.button == SDL_BUTTON_X2) {
        mouseStates_[FORW] = false;
    }
}

/**
 * @brief Возвращает указатель на объект вектора с текущими координатами мыши.
 * 
 * @return Vector2D* Указатель на двумерный вектор позиции (X, Y).
 */
Vector2D* InputHandler::getMousePosition() { return mousePosition_; }

/**
 * @brief Обработчик события перемещения мыши.
 * 
 * Обновляет внутренние координаты вектора mousePosition_ актуальными данными из SDL.
 * 
 * @param event Структура системного события SDL_Event.
 */
void InputHandler::onMouseMotion(SDL_Event event) {
    mousePosition_->set_x(event.motion.x);
    mousePosition_->set_y(event.motion.y);
}

/**
 * @brief Обработчик прерывания при нажатии клавиши клавиатуры.
 * 
 * Запрашивает у SDL актуальный срез состояний всей клавиатуры и сохраняет его во внутренний указатель.
 * 
 * @param event Структура системного события SDL_Event.
 */
void InputHandler::onKeyDown(SDL_Event event) {
    keystates_ = SDL_GetKeyboardState(0);
    if (isKeyDown(SDL_SCANCODE_DOWN)) {
        std::cout << "down" << std::endl;
    }
}

/**
 * @brief Проверяет, зажата ли конкретная клавиша клавиатуры в данный момент.
 * 
 * @param key Скан-код проверяемой клавиши из перечисления SDL_Scancode.
 * @return true Если клавиша удерживается нажатой.
 * @return false Если клавиша не нажата или массив состояний еще не инициализирован.
 */
bool InputHandler::isKeyDown(SDL_Scancode key) {
    if (keystates_ != 0) {
        if (keystates_[key] == true) {
            return true;
        }
        return false;
    }
    return false;
}

/**
 * @brief Центральный шлюз распределения входящих событий ввода.
 * 
 * Принимает сырое событие SDL из главного игрового цикла и перенаправляет его 
 * в соответствующий специализированный приватный метод в зависимости от типа (Mouse/Keyboard).
 * 
 * @param event Структура системного события SDL_Event.
 */
void InputHandler::handle(SDL_Event event) {
    switch (event.type) {
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            onMouseButtonDown(event);
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            onMouseButtonUp(event);
            break;
        case SDL_EVENT_MOUSE_MOTION:
            onMouseMotion(event);
            break;
        case SDL_EVENT_KEY_DOWN:
            onKeyDown(event);
            break;
        
        default:
            break;
    }
}
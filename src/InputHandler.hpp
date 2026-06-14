
#include <SDL3/SDL.h>
#include "Vector2D.hpp"
#include <iostream>
#include <vector>

/**
 * @brief Перечисление для удобного индексирования кнопок мыши.
 * @details Смещено на -1 относительно стандартных констант SDL,
 *          чтобы использовать значения напрямую в качестве индексов массивов (от 0).
 */
enum mouse_buttons {
    /** @brief Левая кнопка мыши. */
    LEFT = SDL_BUTTON_LEFT - 1,
    /** @brief Колесико (средняя кнопка) мыши. */
    MIDDLE = SDL_BUTTON_MIDDLE - 1,
    /** @brief Правая кнопка мыши. */
    RIGHT = SDL_BUTTON_RIGHT - 1,
    /** @brief Боковая кнопка мыши «Назад» (X1). */
    BACK = SDL_BUTTON_X1 - 1,
    /** @brief Боковая кнопка мыши «Вперед» (X2). */
    FORW = SDL_BUTTON_X2 - 1
};

/**
 * @brief Класс-синглтон для обработки ввода от клавиатуры и мыши через SDL.
 * @details Собирает системные события ввода, обновляет внутренние состояния кнопок
 *          и предоставляет глобальный доступ к текущему состоянию устройств ввода.
 */
class InputHandler {
public:
    /**
     * @brief Возвращает единственный экземпляр класса (реализация паттерна Singleton).
     * 
     * @return Указатель на объект InputHandler.
     */
    static InputHandler* Instance() {
        if (instance_ == 0) instance_ = new InputHandler();
        return instance_;
    }

    /**
     * @brief Главный метод обработки входящего события SDL.
     * @details Перенаправляет событие в соответствующие внутренние методы в зависимости от его типа.
     * 
     * @param event Структура события SDL_Event, полученная из очереди событий.
     */
    void handle(SDL_Event event);

    /**
     * @brief Возвращает текущие экранные координаты курсора мыши.
     * 
     * @return Указатель на объект Vector2D с координатами X и Y.
     */
    Vector2D* getMousePosition();

    /**
     * @brief Проверяет, зажата ли конкретная кнопка мыши.
     * 
     * @param buttonNumber Индекс кнопки из перечисления #mouse_buttons.
     * @return true Если кнопка зажата.
     * @return false Если кнопка отпущена.
     */
    bool getMouseButtonState(int buttonNumber);

    /**
     * @brief Проверяет, нажата ли определенная клавиша на клавиатуре.
     * 
     * @param key Скан-код клавиши SDL (SDL_Scancode).
     * @return true Если клавиша удерживается нажатой.
     * @return false Если клавиша отпущена.
     */
    bool isKeyDown(SDL_Scancode key);

private:
    /**
     * @brief Закрытый конструктор для предотвращения создания объектов извне.
     */
    InputHandler();

    /**
     * @brief Закрытый деструктор для контроля жизненного цикла синглтона.
     */
    ~InputHandler();

    /** @brief Статический указатель на единственный экземпляр класса. */
    static InputHandler* instance_;

    /**
     * @brief Обработчик события нажатия кнопки мыши.
     * 
     * @param event Данные события SDL_MOUSEBUTTONDOWN.
     */
    void onMouseButtonDown(SDL_Event event);

    /**
     * @brief Обработчик события отпускания кнопки мыши.
     * 
     * @param event Данные события SDL_MOUSEBUTTONUP.
     */
    void onMouseButtonUp(SDL_Event event);

    /**
     * @brief Обработчик события перемещения мыши.
     * 
     * @param event Данные события SDL_MOUSEMOTION.
     */
    void onMouseMotion(SDL_Event event);

    /**
     * @brief Обработчик события нажатия клавиши клавиатуры.
     * 
     * @param event Данные события SDL_KEYDOWN.
     */
    void onKeyDown(SDL_Event event);

    /** @brief Массив состояний кнопок мыши (true — нажата, false — отпущена). */
    std::vector<bool> mouseStates_;

    /** @brief Текущая позиция мыши на экране. */
    Vector2D* mousePosition_;

    /** @brief Указатель на внутренний массив состояний клавиш клавиатуры SDL. */
    const bool* keystates_ = nullptr;

};

#pragma once
#include <SDL3/SDL.h>
#include <iostream>
#include <string>

/**
 * @brief Базовый класс для всех игровых объектов.
 * @details Содержит основные параметры пространственного положения, размеров,
 *          анимации и текстуры, а также определяет виртуальный интерфейс 
 *          для жизненного цикла сущностей.
 */
class GameObject {
protected:
    /** @brief Идентификатор (тег) текстуры объекта в менеджере ресурсов. */
    std::string texture_tag_;
    /** @brief Индекс текущего кадра анимации. */
    int currentFrame_;
    /** @brief Индекс текущего ряда (строки) на спрайт-листе анимации. */
    int currentRow_;
    /** @brief Координаты X и Y положения объекта в игровом мире. */
    float x_, y_;
    /** @brief Ширина и высота объекта. */
    float w_, h_;

public:
    /**
     * @brief Инициализирует объект начальными параметрами.
     * 
     * @param tag Идентификатор текстуры.
     * @param x Начальная координата X.
     * @param y Начальная координата Y.
     * @param w Ширина объекта.
     * @param h Высота объекта.
     */
    virtual void load(std::string tag, float x, float y, float w, float h);
    
    /**
     * @brief Отрисовывает объект на экране.
     * 
     * @param renderer Указатель на активный рендерер SDL.
     */
    virtual void draw(SDL_Renderer* renderer);
    
    /**
     * @brief Обновляет логическое состояние объекта (физика, анимация) каждый кадр.
     */
    virtual void update();
    
    /**
     * @brief Освобождает ресурсы, связанные с объектом.
     */
    virtual void clean();

    /**
     * @brief Возвращает границы объекта в виде прямоугольника SDL_FRect.
     * 
     * @return Прямоугольник, описывающий положение и размеры объекта.
     */
    SDL_FRect getBounds() const;
    
    /**
     * @brief Возвращает тег текстуры объекта.
     * 
     * @return Строка с идентификатором текстуры.
     */
    std::string getTextureTag() const;
};
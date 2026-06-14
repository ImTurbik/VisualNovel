#pragma once
#include "GameObject.hpp"

/**
 * @brief Класс, представляющий неигрового персонажа (NPC) в игре.
 * @details Наследуется от базового класса GameObject и переопределяет его ключевые
 *          виртуальные методы для настройки поведения, отрисовки и логики NPC.
 */
class Npc: public GameObject {
public:
    /**
     * @brief Загружает и инициализирует параметры NPC.
     * 
     * @param tag Идентификатор текстуры персонажа.
     * @param x Начальная координата X на карте.
     * @param y Начальная координата Y на карте.
     * @param w Ширина спрайта персонажа.
     * @param h Высота спрайта персонажа.
     */
    void load(std::string tag, float x, float y, float w, float h);
    
    /**
     * @brief Отрисовывает текстуру NPC с учетом текущего кадра анимации.
     * 
     * @param renderer Указатель на активный рендерер SDL.
     */
    void draw(SDL_Renderer* renderer);
    
    /**
     * @brief Обновляет внутреннюю логику NPC (например, анимацию или ИИ) каждый кадр.
     */
    void update();
    
    /**
     * @brief Освобождает специфичные ресурсы, связанные с данным NPC.
     */
    void clean();
};
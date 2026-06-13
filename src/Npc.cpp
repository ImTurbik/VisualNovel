/**
 * @file Npc.cpp
 * @brief Реализация класса неигрового персонажа (NPC).
 */

#include "Npc.hpp"
#include <iostream>

/**
 * @brief Загружает и инициализирует параметры неигрового персонажа.
 * 
 * Метод полностью делегирует инициализацию пространственных координат и 
 * текстурного тега базовому классу GameObject.
 * 
 * @param tag Уникальный строковый идентификатор текстуры NPC в TextureManager.
 * @param x Начальная координата NPC по оси X.
 * @param y Начальная координата NPC по оси Y.
 * @param w Ширина спрайта NPC на экране.
 * @param h Высота спрайта NPC на экране.
 */
void Npc::load(std::string tag, float x, float y, float w, float h) {
    GameObject::load(tag, x, y, w, h);
}

/**
 * @brief Отрисовывает спрайт NPC на экране.
 * 
 * Вызывает базовый метод отрисовки кадра из класса GameObject, используя 
 * текущие параметры анимации и рендерер SDL.
 * 
 * @param renderer Указатель на активный контекст рендеринга SDL.
 */
void Npc::draw(SDL_Renderer* renderer) {
    GameObject::draw(renderer);
}

/**
 * @brief Обновляет логику и состояние NPC за один кадр.
 * 
 * Вызывает базовое обновление из GameObject. Метод готов для дальнейшего 
 * расширения (например, для добавления патрулирования, смены направления взгляда или анимации idle).
 */
void Npc::update() {
    GameObject::update();
}

/**
 * @brief Освобождает ресурсы NPC и выводит отладочную информацию.
 * 
 * Сначала выполняет базовую очистку через GameObject::clean(), после чего 
 * выводит в консоль специализированное сообщение об успешном удалении сущности NPC.
 */
void Npc::clean() {
    GameObject::clean();
    std::cout << "NPC cleaned" << std::endl;
}
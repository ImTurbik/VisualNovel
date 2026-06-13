/**
 * @class Collider
 * @brief Класс для обработки столкновений и управления границами объектов.
 *
 * Предоставляет функционал для создания прямоугольных хитбоксов,
 * проверки их пересечения и визуализации для отладки.
 */
class Collider {
public:
    /**
     * @brief Конструктор для инициализации границ коллайдера.
     * 
     * @param x Координата по оси X левого верхнего угла.
     * @param y Координата по оси Y левого верхнего угла.
     * @param w Ширина коллайдера.
     * @param h Высота коллайдера.
     */
    Collider(float x, float y, float w, float h) {
        bounds.x = x;
        bounds.y = y;
        bounds.w = w;
        bounds.h = h;
    }

    /**
     * @brief Проверяет столкновение (пересечение) двух прямоугольников.
     * 
     * Использует алгоритм AABB (Axis-Aligned Bounding Box) для определения пересечения.
     * 
     * @param rectA Первый прямоугольник для проверки.
     * @param rectB Второй прямоугольник для проверки.
     * @return true Если прямоугольники пересекаются.
     * @return false Если прямоугольники не пересекаются.
     */
    bool checkCollision(const SDL_FRect& rectA, const SDL_FRect& rectB) {
        if (rectA.x + rectA.w <= rectB.x) return false;
        if (rectA.x >= rectB.x + rectB.w) return false;
        if (rectA.y + rectA.h <= rectB.y) return false;
        if (rectA.y >= rectB.y + rectB.h) return false;

        return true;
    }

    /**
     * @brief Отображает границы коллайдера в виде красного прямоугольника.
     * 
     * Используется для визуальной отладки геометрии объектов на экране.
     * 
     * @param renderer Указатель на активный рендерер SDL.
     */
    void drawDebug(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderRect(renderer, &bounds);
    }

private:
    SDL_FRect bounds; /**< Прямоугольные границы данного коллайдера. */
};

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <map>
#include <string>

/**
 * @brief Менеджер текстур для загрузки, хранения и отрисовки графических ресурсов SDL.
 * @details Реализует паттерн проектирования Singleton (синглтон Майерса). 
 *          Управляет временем жизни текстур и предоставляет удобный доступ 
 *          к ним по строковым идентификаторам (тегам).
 */
class TextureManager {
private:
    /**
     * @brief Конструктор по умолчанию (скрыт для реализации паттерна Singleton).
     */
    TextureManager() = default;
    
    // Запрет копирования и перемещения для обеспечения единственности экземпляра
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;
    TextureManager(TextureManager&&) = delete;
    TextureManager& operator=(TextureManager&&) = delete;

    /** @brief Ассоциативный массив (карта) для хранения загруженных текстур по их уникальным тегам. */
    std::map<std::string, SDL_Texture*> textureMap_;

public:
    /**
     * @brief Загружает изображение из файла и сохраняет его в карту текстур.
     * 
     * @param fileName Путь к файлу изображения на диске.
     * @param tag Уникальное имя (тег), которое будет связано с этой текстурой.
     * @param renderer Указатель на активный рендерер SDL для создания текстуры.
     * @return true Если текстура успешно загружена и добавлена.
     * @return false Если произошла ошибка при загрузке или создании текстуры.
     */
    bool load(std::string fileName, std::string tag, SDL_Renderer* renderer);
    
    /**
     * @brief Отрисовывает статичную текстуру целиком на экране.
     * 
     * @param tag Тег предварительно загруженной текстуры.
     * @param x Координата X на экране, где будет расположен левый верхний угол.
     * @param y Координата Y на экране, где будет расположен левый верхний угол.
     * @param width Ширина области отрисовки.
     * @param height Высота области отрисовки.
     * @param renderer Указатель на активный рендерер SDL.
     * @param flip Режим зеркального отображения текстуры (по умолчанию без отражения).
     */
    void draw(std::string tag, float x, float y, float width, float height,
              SDL_Renderer* renderer, SDL_FlipMode flip = SDL_FLIP_NONE);
    
    /**
     * @brief Отрисовывает конкретный кадр анимации из спрайт-листа (сетки кадров).
     * 
     * @param tag Тег предварительно загруженной текстуры.
     * @param x Координата X на экране для левого верхнего угла кадра.
     * @param y Координата Y на экране для левого верхнего угла кадра.
     * @param width Ширина одного кадра на экране.
     * @param height Высота одного кадра на экране.
     * @param currentRow Индекс текущего ряда (строки) на спрайт-листе (индексация с 1 или 0 зависит от вашей логики).
     * @param currentFrame Индекс текущего кадра в ряду (столбца).
     * @param renderer Указатель на активный рендерер SDL.
     * @param flip Режим зеркального отображения кадра (по умолчанию без отражения).
     */
    void drawFrame(std::string tag, float x, float y, float width, float height,
                   int currentRow, int currentFrame, SDL_Renderer* renderer,
                   SDL_FlipMode flip = SDL_FLIP_NONE);

    /**
     * @brief Возвращает ссылку на единственный экземпляр класса TextureManager.
     * 
     * @return Ссылка на объект TextureManager.
     */
    static TextureManager& Instance() {
      static TextureManager Instance_;
      return Instance_;
    }   
    
    /**
     * @brief Удаляет конкретную текстуру из карты по её тегу и освобождает её память.
     * 
     * @param tag Тег удаляемой текстуры.
     */
    void clearFromTextureMap(std::string tag);
    
    /**
     * @brief Очищает всю карту текстур, корректно уничтожая каждый объект SDL_Texture.
     */
    void clean();
};